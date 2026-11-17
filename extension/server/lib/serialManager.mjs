/**
 * serialManager.mjs
 * ─────────────────
 * Serial I/O via `arduino-cli monitor` — no native bindings required.
 *
 * arduino-cli is already a hard requirement for compile/upload, so spawning
 * it for serial monitoring adds zero new dependencies and works identically
 * on macOS, Linux, and Windows.
 */

import { spawn } from "node:child_process";
import { OFFLINE_ENV, killTree, notFoundMessage, resolveArduinoCli } from "./cli.mjs";

/** Longest unfinished line kept while waiting for its newline. */
const MAX_PARTIAL_CHARS = 16_384;

export class SerialManager {
  /**
   * @param {{ onLine?: (line: string) => void, onReset?: () => void }} [hooks]
   *   onLine gets every complete line (the Plotter's buffer); onReset runs
   *   when a session opens or its lines are forgotten.
   */
  constructor({ onLine, onReset } = {}) {
    this._onLine = onLine;
    this._onReset = onReset;
    /** @type {import("node:child_process").ChildProcess|null} */
    this._proc  = null;
    /** @type {string[]} */
    this._buffer = [];
    this._maxBufferLines = 500;
    this.isOpen  = false;
    this._meta   = { path: null, baudRate: null };
    /** Accumulates bytes that haven't yet formed a complete line. */
    this._partial = "";
  }

  /**
   * Open a serial connection via `arduino-cli monitor`.
   * @param {{ path: string, baudRate?: number, delimiter?: string }} args
   *   `delimiter` is accepted for API compatibility but ignored — line
   *   splitting is handled by splitting on \r?\n from stdout.
   */
  async open({ path, baudRate = 9600 }) {
    await this.close();
    const cli = resolveArduinoCli();
    if (!cli) throw new Error(notFoundMessage());

    const proc = spawn(cli.path, [
      "monitor",
      "-p", path,
      "--config", `baudrate=${baudRate}`,
      "--quiet",          // suppress the connection banner
      "--additional-urls", "", // no third-party index at startup (see cli.mjs)
    ], {
      stdio: ["pipe", "pipe", "pipe"],
      shell: false,
      windowsHide: true,
      env: { ...process.env, ...OFFLINE_ENV },
    });

    // Lines from a previous session (or a previous sketch) never leak into this one.
    this._buffer = [];
    this._partial = "";
    this._plotSkip = true;
    this._onReset?.();

    proc.stdout.on("data", (chunk) => {
      const text  = this._partial + String(chunk);
      const lines = text.split(/\r?\n/);
      // The last element is either "" (if chunk ended with \n) or a partial line.
      // A board that never sends a newline (binary data, wrong baud rate) must not grow this without end.
      this._partial = (lines.pop() ?? "").slice(-MAX_PARTIAL_CHARS);
      for (const line of lines) {
        this._buffer.push(line);
        // The first line after open or clear can start mid-line ("dom:837" from
        // "random:837"), and its fragment would become a label of its own. The
        // Plotter skips it; serialRead still gets it.
        if (this._plotSkip) this._plotSkip = false;
        else this._onLine?.(line);
        if (this._buffer.length > this._maxBufferLines) {
          this._buffer.splice(0, this._buffer.length - this._maxBufferLines);
        }
      }
    });

    proc.on("exit", () => {
      if (this._proc === proc) {
        this._proc  = null;
        this.isOpen = false;
      }
    });

    // Give the process 500 ms to either stabilise or crash.
    // An early exit (bad port, permission denied, port in use) rejects here
    // so callers get a meaningful error instead of a silent open.
    await new Promise((resolve, reject) => {
      let settled = false;
      let stderrBuf = "";

      proc.stderr.on("data", (d) => { stderrBuf += String(d); });

      const timer = setTimeout(() => {
        if (!settled) { settled = true; resolve(); }
      }, 500);

      proc.once("exit", (code) => {
        clearTimeout(timer);
        if (!settled) {
          settled = true;
          reject(new Error(
            stderrBuf.trim() || `arduino-cli monitor exited early (code ${code})`
          ));
        }
      });

      // A spawn failure (missing binary, blocked by the OS) emits "error", not "exit".
      proc.once("error", (err) => {
        clearTimeout(timer);
        if (!settled) {
          settled = true;
          reject(err);
        }
      });
    });

    this._proc  = proc;
    this.isOpen = true;
    this._meta  = { path, baudRate };
  }

  /**
   * Write data to the serial port (forwarded to arduino-cli monitor's stdin).
   * @param {{ data: string }} args
   */
  async write({ data }) {
    if (!this._proc || !this.isOpen) throw new Error("Serial port is not open");
    await new Promise((resolve, reject) => {
      this._proc.stdin.write(String(data), (err) => (err ? reject(err) : resolve()));
    });
  }

  /**
   * Return buffered lines. Clears the buffer by default.
   * @param {{ clear?: boolean }} [args]
   */
  read({ clear } = {}) {
    const lines = this._buffer.slice();
    if (clear !== false) this._buffer = [];
    return lines;
  }

  /**
   * Close the serial connection and wait until the monitor process has
   * exited, so the OS has released the port before an upload opens it.
   */
  async close() {
    if (!this._proc) return;
    const proc    = this._proc;
    this._proc    = null;
    this.isOpen   = false;
    this._meta    = { path: null, baudRate: null };
    this._partial = "";
    if (proc.exitCode !== null || proc.signalCode !== null) return;
    const exited = new Promise((r) => proc.once("exit", r));
    try { proc.kill("SIGTERM"); } catch { /* already gone */ }
    const timedOut = await Promise.race([
      exited.then(() => false),
      new Promise((r) => setTimeout(() => r(true), 1500)),
    ]);
    if (timedOut) {
      killTree(proc);
      await Promise.race([exited, new Promise((r) => setTimeout(r, 1000))]);
    }
  }

  /** Forget buffered lines (after an upload, the old sketch's output is stale). */
  clear() {
    this._buffer = [];
    this._partial = "";
    this._plotSkip = true;
    this._onReset?.();
  }

  /** Current status snapshot (mirrors the old serialport API shape). */
  status() {
    return {
      isOpen:       this.isOpen,
      path:         this._meta.path,
      baudRate:     this._meta.baudRate,
      bufferedLines: this._buffer.length,
    };
  }
}
