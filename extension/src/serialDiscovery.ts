/**
 * serialDiscovery.ts
 * ──────────────────
 * Tells the editor when a USB serial port appears or disappears, without
 * polling (ADR 0014). It runs arduino-cli's own `serial-discovery` tool
 * (Arduino Pluggable Discovery, protocol 1) as one long-lived child process
 * and listens to its `add` / `remove` events. The tool is quiet while nothing
 * changes, so no `arduino-cli board list` runs until a port really changes.
 *
 * This module never opens a port, reads serial data or runs arduino-cli
 * itself: it only calls `onChange`, and the caller rescans the boards.
 */

import * as vscode from "vscode";
import * as fs from "node:fs";
import * as path from "node:path";
import { spawn, type ChildProcess } from "node:child_process";

import { cliDataDir, onFirstRun } from "../server/lib/cli.mjs";

const DEBOUNCE_MS = 300;
const LOCATE_RETRY_MS = 15_000;
const STOP_WAIT_MS = 500;
const BACKOFF_MAX_MS = 30_000;
const DEATH_WINDOW_MS = 120_000;
const DEATHS_ALLOWED = 5;
const ERRORS_IN_A_ROW = 3;
/** Uptime after which a crash counts as new: the restart wait starts again at 1 s. */
const STABLE_UPTIME_MS = 60_000;
/** After START_SYNC the tool re-adds every present port; collect them this long, then compare. */
const RESYNC_MS = 1000;

export interface SerialDiscoveryOptions {
  output: vscode.OutputChannel;
  /** Called (debounced) after a USB serial port appeared or disappeared. */
  onChange: (why: string) => void;
  /** While true, port events are ignored (an upload re-enumerates the board). */
  isUploading: () => boolean;
}

/** Splits the tool's stdout (pretty-printed JSON objects, back to back) into objects. */
export class JsonObjectStream {
  private buf = "";
  private depth = 0;
  private inString = false;
  private escaped = false;
  private start = -1;

  /** Feed a chunk; returns every object completed by it. Bad JSON is skipped. */
  push(chunk: string): any[] {
    const out: any[] = [];
    const base = this.buf.length;
    this.buf += chunk;
    for (let i = base; i < this.buf.length; i++) {
      const ch = this.buf[i];
      if (this.inString) {
        if (this.escaped) this.escaped = false;
        else if (ch === "\\") this.escaped = true;
        else if (ch === '"') this.inString = false;
        continue;
      }
      if (this.depth > 0 && ch === '"') {
        this.inString = true;
      } else if (ch === "{") {
        if (this.depth === 0) this.start = i;
        this.depth++;
      } else if (ch === "}" && this.depth > 0) {
        this.depth--;
        if (this.depth === 0) {
          try {
            out.push(JSON.parse(this.buf.slice(this.start, i + 1)));
          } catch {
            /* not JSON: skip it */
          }
          this.buf = this.buf.slice(i + 1);
          i = -1;
          this.start = -1;
        }
      }
    }
    if (this.depth === 0) this.buf = ""; // text between objects is noise
    return out;
  }
}

function semverKey(v: string): number[] {
  return v.split(/[.-]/).slice(0, 3).map((n) => parseInt(n, 10) || 0);
}

/** Newest installed serial-discovery, or null when the first run has not fetched it yet. */
export async function locateSerialDiscovery(): Promise<{ exe: string; version: string } | null> {
  const dataDir = await cliDataDir();
  if (!dataDir) return null;
  const toolDir = path.join(dataDir, "packages", "builtin", "tools", "serial-discovery");
  const exeName = process.platform === "win32" ? "serial-discovery.exe" : "serial-discovery";
  let versions: string[];
  try {
    versions = fs.readdirSync(toolDir);
  } catch {
    return null;
  }
  versions.sort((a, b) => {
    const ka = semverKey(a);
    const kb = semverKey(b);
    for (let i = 0; i < 3; i++) if (ka[i] !== kb[i]) return kb[i] - ka[i];
    return 0;
  });
  for (const version of versions) {
    const exe = path.join(toolDir, version, exeName);
    try {
      if (fs.statSync(exe).isFile()) return { exe, version };
    } catch {
      /* try the next version */
    }
  }
  return null;
}

export function startSerialDiscovery(opts: SerialDiscoveryOptions): vscode.Disposable {
  const { output } = opts;
  let disposed = false;
  let child: ChildProcess | null = null;
  let locateTimer: ReturnType<typeof setTimeout> | null = null;
  let restartTimer: ReturnType<typeof setTimeout> | null = null;
  let debounce: ReturnType<typeof setTimeout> | null = null;
  let pendingWhy: string[] = [];
  let missingLogged = false;
  let errorsInARow = 0;
  let backoffMs = 1000;
  let deaths: number[] = [];
  let gaveUp = false;
  let startedAt = 0;
  /**
   * USB ports (address → vid:pid) as last reported. Kept across restarts, so a
   * restart fires only for ports that really changed while the tool was down.
   * Addresses are tracked from `add`, because `remove` carries no VID.
   */
  const usbPorts = new Map<string, string>();
  /** Ports seen since the latest START_SYNC; null when not resyncing. */
  let resync: Map<string, string> | null = null;
  let resyncTimer: ReturnType<typeof setTimeout> | null = null;

  const log = (s: string) => output.appendLine(`[Grease] ${s}`);

  const fire = (why: string) => {
    if (opts.isUploading()) return;
    pendingWhy.push(why);
    if (debounce) clearTimeout(debounce);
    debounce = setTimeout(() => {
      debounce = null;
      const all = pendingWhy.join(", ");
      pendingWhy = [];
      if (!disposed && !opts.isUploading()) opts.onChange(all);
    }, DEBOUNCE_MS);
  };

  const onEvent = (ev: any) => {
    if (!ev || typeof ev !== "object") return;
    if (ev.error === true) {
      errorsInARow++;
      log(`serial-discovery error: ${ev.message ?? JSON.stringify(ev)}`);
      if (errorsInARow >= ERRORS_IN_A_ROW && child) {
        log("serial-discovery: 3 errors in a row, restarting it.");
        child.kill();
      }
      return;
    }
    errorsInARow = 0;
    const address: string | undefined = ev.port?.address;
    switch (ev.eventType) {
      case "start_sync":
        resync = new Map();
        if (resyncTimer) clearTimeout(resyncTimer);
        resyncTimer = setTimeout(endResync, RESYNC_MS);
        break;
      case "add": {
        const p = ev.port?.properties ?? {};
        if (!address || !p.vid) return;
        const id = `${String(p.vid).replace(/^0x/i, "")}:${String(p.pid ?? "").replace(/^0x/i, "")}`.toLowerCase();
        if (resync) {
          resync.set(address, id);
          return;
        }
        if (usbPorts.get(address) === id) return;
        usbPorts.set(address, id);
        fire(`add ${address} ${id}`);
        break;
      }
      case "remove":
        if (!address) return;
        if (resync) {
          resync.delete(address);
          return;
        }
        if (!usbPorts.delete(address)) return;
        fire(`remove ${address}`);
        break;
    }
  };

  /** Compares the ports re-added after START_SYNC with the ones known before. */
  const endResync = () => {
    resyncTimer = null;
    const now = resync;
    resync = null;
    if (!now || disposed) return;
    for (const [address, id] of now) {
      if (usbPorts.get(address) !== id) {
        usbPorts.set(address, id);
        fire(`add ${address} ${id}`);
      }
    }
    for (const address of [...usbPorts.keys()]) {
      if (!now.has(address)) {
        usbPorts.delete(address);
        fire(`remove ${address}`);
      }
    }
  };

  const scheduleLocate = () => {
    if (disposed || locateTimer) return;
    locateTimer = setTimeout(() => {
      locateTimer = null;
      void launch();
    }, LOCATE_RETRY_MS);
  };

  const onExit = (why: string) => {
    child = null;
    // usbPorts stays: the next START_SYNC is compared against it (endResync).
    if (resyncTimer) clearTimeout(resyncTimer);
    resyncTimer = null;
    resync = null;
    if (disposed || gaveUp) return;
    const now = Date.now();
    if (startedAt && now - startedAt >= STABLE_UPTIME_MS) backoffMs = 1000;
    deaths = deaths.filter((t) => now - t < DEATH_WINDOW_MS);
    deaths.push(now);
    if (deaths.length > DEATHS_ALLOWED) {
      gaveUp = true;
      const msg = "serial-discovery keeps dying: boards are found only by Refresh.";
      log(msg);
      void vscode.window.showWarningMessage(`Grease: ${msg}`, "Refresh").then((pick) => {
        if (pick === "Refresh") void vscode.commands.executeCommand("arduinoMcp.refreshPortsBoards");
      });
      return;
    }
    log(`serial-discovery stopped (${why}); restarting in ${backoffMs / 1000} s.`);
    restartTimer = setTimeout(() => {
      restartTimer = null;
      void launch();
    }, backoffMs);
    backoffMs = Math.min(backoffMs * 2, BACKOFF_MAX_MS);
  };

  const launch = async () => {
    if (disposed || child || gaveUp) return;
    const found = await locateSerialDiscovery();
    if (disposed || child) return;
    if (!found) {
      if (!missingLogged) {
        log("USB port finder not downloaded yet (arduino-cli first run, 14 MB). Boards will appear by themselves when it finishes.");
        missingLogged = true;
      }
      scheduleLocate();
      return;
    }
    missingLogged = false;
    let proc: ChildProcess;
    try {
      proc = spawn(found.exe, [], { stdio: ["pipe", "pipe", "pipe"], windowsHide: true });
    } catch (e) {
      onExit(e instanceof Error ? e.message : String(e));
      return;
    }
    child = proc;
    startedAt = Date.now();
    errorsInARow = 0;
    const stream = new JsonObjectStream();
    proc.stdout?.setEncoding("utf8");
    proc.stdout?.on("data", (s: string) => {
      for (const ev of stream.push(s)) onEvent(ev);
    });
    proc.stderr?.setEncoding("utf8");
    proc.stderr?.on("data", (s: string) => {
      const line = s.trim();
      if (line) log(`serial-discovery: ${line}`);
    });
    proc.stdin?.on("error", () => { /* the exit handler reports it */ });
    let exited = false;
    const done = (why: string) => {
      if (exited) return;
      exited = true;
      if (child === proc) onExit(why);
    };
    proc.on("error", (e) => done(e.message));
    proc.on("exit", (code, signal) => done(signal ? `signal ${signal}` : `exit ${code}`));
    proc.stdin?.write('HELLO 1 "grease"\nSTART_SYNC\n');
    log(`serial-discovery on (${found.version})`);
  };

  const stopListening = onFirstRun((e) => {
    if (e.phase === "done" && !child && !disposed && !gaveUp) {
      if (locateTimer) clearTimeout(locateTimer);
      locateTimer = null;
      void launch();
    }
  });

  void launch();

  return {
    dispose: () => {
      disposed = true;
      stopListening();
      if (locateTimer) clearTimeout(locateTimer);
      if (restartTimer) clearTimeout(restartTimer);
      if (resyncTimer) clearTimeout(resyncTimer);
      if (debounce) clearTimeout(debounce);
      const proc = child;
      child = null;
      if (!proc) return;
      try {
        proc.stdin?.write("STOP\nQUIT\n");
        proc.stdin?.end();
      } catch {
        /* already gone */
      }
      const killer = setTimeout(() => {
        try { proc.kill(); } catch { /* already gone */ }
      }, STOP_WAIT_MS);
      proc.once("exit", () => clearTimeout(killer));
    },
  };
}
