/**
 * serverProcess.ts
 * ────────────────
 * Finds the computer's AI Tether (extension/server/server.mjs → dist/server.mjs)
 * or starts it, and returns a handle the extension uses to talk to it.
 *
 * One AI Tether per computer (ADR 0008):
 *   - every editor window, and the stdio bridge, use the AI Tether named in
 *     ~/.grease/extension/mcp-auth.json when its /health answers;
 *   - none answers: start one as a detached process, so it outlives this
 *     window and serves agents (Claude Desktop, Codex) with no editor open;
 *   - it exits by itself after ~10 minutes with no client;
 *   - a newer build replaces an older one that is not busy.
 *
 * Why a separate process and not in-process?
 *   AI Tether uses the `serialport` native module. Running it inside the
 *   extension host can crash the editor on an ABI mismatch, and a separate
 *   process can serve every window and agent at once.
 */

import { spawn } from "node:child_process";
import * as path from "node:path";
import * as os from "node:os";
import * as fs from "node:fs";
import { randomBytes } from "node:crypto";
import type { ExtensionContext, OutputChannel } from "vscode";
import { editorHeaders } from "./serverHttpClient";

declare const GREASE_BUILD: string;
/** Build time (ms) stamped by scripts/build.mjs; AI Tether reports the same stamp. */
const BUILD = typeof GREASE_BUILD === "string" ? GREASE_BUILD : "0";

const GREASE_EXT_DIR = path.join(os.homedir(), ".grease", "extension");
const AUTH_FILE = path.join(GREASE_EXT_DIR, "mcp-auth.json");
/** AI Tether's stdout + stderr: it outlives the window, so no Output channel. */
export const TETHER_LOG = path.join(GREASE_EXT_DIR, "ai-tether.log");

function delay(ms: number): Promise<void> {
  return new Promise((r) => setTimeout(r, ms));
}

interface Health {
  version: string;
  build: string;
  port: number;
  pid: number;
  busy: boolean;
}

/** /health of the AI Tether on this port, or null when none answers. */
async function health(port: number): Promise<Health | null> {
  try {
    const res = await fetch(`http://127.0.0.1:${port}/health`, { signal: AbortSignal.timeout(1000) });
    const j: any = await res.json();
    return j?.name === "grease-ai-tether" ? (j as Health) : null;
  } catch {
    return null;
  }
}

function readAuth(): { key: string | null; port: number | null } {
  try {
    const j = JSON.parse(fs.readFileSync(AUTH_FILE, "utf8"));
    return { key: typeof j.key === "string" ? j.key : null, port: Number(j.port) || null };
  } catch {
    return { key: null, port: null };
  }
}

/** The AI Tether that mcp-auth.json names, when it answers. */
async function running(): Promise<Health | null> {
  const { port } = readAuth();
  return port ? health(port) : null;
}

export interface ServerHandle {
  port: number;
  authKey: string;
  /** Turns AI Tether off for the whole computer: every window and agent. */
  stop: () => Promise<void>;
}

function handleFor(port: number, authKey: string, output: OutputChannel): ServerHandle {
  return {
    port,
    authKey,
    stop: async () => {
      const before = await health(port);
      if (!before) return;
      try {
        await fetch(`http://127.0.0.1:${port}/shutdown`, {
          method: "POST",
          headers: editorHeaders(authKey),
          signal: AbortSignal.timeout(2000),
        });
      } catch {
        /* checked below */
      }
      for (let i = 0; i < 16; i++) {
        if ((await health(port))?.pid !== before.pid) return;
        await delay(250);
      }
      output.appendLine(`AI Tether (pid ${before.pid}) did not stop in 4 s.`);
    },
  };
}

/**
 * Use the computer's AI Tether, starting it when none runs.
 *
 * Env passed to a new one:
 *   - `MCP_PORT`          — first port to try (it moves up past other programs)
 *   - `MCP_HOST`          — loopback, never exposed externally
 *   - `MCP_AUTH_KEY`      — key to use when ~/.grease/extension/mcp-auth.json has none
 *   - `MCP_IDE_NAME`      — the editor that started it
 *   - `GREASE_STARTED_BY` — "editor": the editor installs arduino-cli, not AI Tether
 */
export async function connectTether(
  context: ExtensionContext,
  output: OutputChannel,
  preferredPort = 3333,
  ideName = "Visual Studio Code",
): Promise<ServerHandle> {
  const serverPath = context.asAbsolutePath(path.join("dist", "server.mjs"));

  const found = await running();
  if (found) {
    const key = readAuth().key;
    if (key && (Number(found.build) >= Number(BUILD) || found.busy)) {
      output.appendLine(`Using AI Tether ${found.version} on port ${found.port} (pid ${found.pid}), shared by this computer.`);
      return handleFor(found.port, key, output);
    }
    if (key) {
      output.appendLine(`Replacing AI Tether ${found.version} (older build, pid ${found.pid}) with this one.`);
      await handleFor(found.port, key, output).stop();
    }
  }

  // Run AI Tether on the editor's own Node (Electron in Node mode), so VS Code
  // users need no separate Node.js install. ARDUINO_MCP_NODE_PATH overrides.
  const nodePath = process.env.ARDUINO_MCP_NODE_PATH || process.execPath;
  const authKey = readAuth().key ?? randomBytes(24).toString("hex");

  fs.mkdirSync(GREASE_EXT_DIR, { recursive: true });
  try {
    if (fs.statSync(TETHER_LOG).size > 1_000_000) fs.renameSync(TETHER_LOG, `${TETHER_LOG}.old`);
  } catch {
    /* no log yet */
  }
  const log = fs.openSync(TETHER_LOG, "a");
  const child = spawn(nodePath, [serverPath], {
    // Home, not the extension folder: Windows cannot delete a folder that is
    // a running process's working directory, which would block updates.
    cwd: os.homedir(),
    env: {
      ...process.env,
      MCP_PORT: String(preferredPort),
      MCP_HOST: "127.0.0.1",
      MCP_AUTH_KEY: authKey,
      MCP_IDE_NAME: ideName,
      ELECTRON_RUN_AS_NODE: "1",
      GREASE_STARTED_BY: "editor",
    },
    detached: true,
    stdio: ["ignore", log, log],
    shell: false,
    windowsHide: true,
  });
  child.unref();
  fs.closeSync(log);
  let exitCode: number | null = null;
  child.once("exit", (code) => (exitCode = code));

  // Ours, or one another window started at the same moment (ours then leaves).
  for (let i = 0; i < 40; i++) {
    await delay(250);
    const now = await running();
    const key = readAuth().key;
    if (now && key && now.pid !== found?.pid) {
      output.appendLine(`AI Tether ${now.version} on port ${now.port} (pid ${now.pid}) · log ${TETHER_LOG}`);
      return handleFor(now.port, key, output);
    }
    if (exitCode !== null && exitCode !== 0) break;
  }
  throw new Error(`AI Tether did not start${exitCode !== null ? ` (exit code ${exitCode})` : ""}. See ${TETHER_LOG}.`);
}
