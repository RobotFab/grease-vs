/**
 * serverHttpClient.ts
 * ───────────────────
 * Tiny REST client for the bundled MCP server (the Node process spawned in
 * src/serverProcess.ts, code in extension/server/server.mjs).
 *
 * Everything in the extension that needs to *talk to* the running server goes
 * through this module — board target sync, serial port open/read/write/close,
 * and health checks. The MCP `/mcp` JSON-RPC endpoint is handled separately
 * by AI clients; we only use the REST surface here.
 */

/** Base URL of the running server. Updated whenever the server actually binds a port. */
let baseUrl = "http://127.0.0.1:3333";

/**
 * Shared-secret auth key sent as the `x-grease-auth` header.
 * Set by setAuthKey() once the server starts and reports its key.
 */
let _authKey = "";

export function setAuthKey(key: string): void {
  _authKey = key;
}

export function setServerBaseUrl(url: string): void {
  const clean = String(url || "").trim();
  if (!clean) return;
  baseUrl = clean.replace(/\/$/, "");
}

/**
 * Auth plus `x-grease-client: editor`, so AI Tether does not count the
 * extension's own calls as an agent at work.
 */
export function editorHeaders(authKey = _authKey): Record<string, string> {
  return { "x-grease-auth": authKey, "x-grease-client": "editor" };
}

/** POST JSON to a server path and return the parsed JSON response. */
async function postJson<T = any>(path: string, body?: unknown): Promise<T> {
  const res = await fetch(`${baseUrl}${path}`, {
    method: "POST",
    headers: { "content-type": "application/json", ...editorHeaders() },
    body: JSON.stringify(body ?? {}),
  });
  if (!res.ok) throw new Error(`HTTP ${res.status} ${path}`);
  return (await res.json()) as T;
}

/** GET /health — returns `{ ok, name, port }`. Open endpoint, but we still send auth. */
export async function getHealth(): Promise<any> {
  const res = await fetch(`${baseUrl}/health`, { headers: editorHeaders() });
  if (!res.ok) throw new Error(`HTTP ${res.status} /health`);
  return await res.json();
}

/**
 * Push the user's chosen (port, fqbn) to the MCP server so AI tool calls like
 * `compileSketch` and `uploadSketch` know what hardware to talk to.
 * Swallows errors silently — the server may still be coming up.
 * `auto` marks the extension's own automatic pick, which AI Tether ignores
 * while an agent's `setTarget` choice is in force.
 */
export async function syncTargetToServer(
  target: { port: string | null; fqbn: string | null } | null,
  opts: { auto?: boolean; chipFix?: boolean } = {},
): Promise<void> {
  try {
    await postJson("/target", { port: target?.port ?? null, fqbn: target?.fqbn ?? null, auto: !!opts.auto, chipFix: !!opts.chipFix });
  } catch {
    /* server may not be ready */
  }
}

/**
 * Which ESP32 chip is on a port (AI Tether reads it once per board; the first
 * read resets the board). Null when AI Tether is down or skipped the port.
 */
export async function chipOf(board: { port: string; usbId: string; serialNumber?: string | null }): Promise<{
  chip: string | null;
  mcu: string | null;
  fqbn: string | null;
} | null> {
  try {
    const r = await postJson("/chip", board);
    return r?.skipped ? null : r;
  } catch {
    return null;
  }
}

export async function serialOpen(args: { path: string; baudRate: number; delimiter?: string }) {
  return postJson("/serial/open", args);
}

export async function serialClose() {
  return postJson("/serial/close", {});
}

export async function serialWrite(args: { data: string }) {
  return postJson("/serial/write", args);
}

export async function serialRead(): Promise<{
  ok: boolean;
  lines: string[];
  serial: { isOpen: boolean; path?: string; baudRate?: number };
}> {
  return postJson("/serial/read", {});
}

/** One parsed serial line in AI Tether's plot buffer. */
export interface PlotRow {
  seq: number;
  t: number;
  v: Record<string, number>;
}

/**
 * The Plotter's rows after `since` (ADR 0009). Unlike /serial/read this takes
 * nothing away from other readers. `cleared`: drop the rows held so far.
 */
export async function plotRead(since: number, clear = false): Promise<{
  ok: boolean;
  seq: number;
  cleared: boolean;
  labels: string[];
  rows: PlotRow[];
  serial: { isOpen: boolean; path?: string; baudRate?: number };
}> {
  return postJson("/plot", { since, clear });
}

export async function postCompile(sketchPath: string): Promise<{
  ok: boolean;
  success: boolean;
  stdout: string;
  stderr: string;
  exitCode: number | null;
}> {
  return postJson("/compile", { sketchPath });
}

export async function postUpload(sketchPath: string): Promise<{
  ok: boolean;
  success: boolean;
  stdout: string;
  stderr: string;
  exitCode: number | null;
}> {
  return postJson("/upload", { sketchPath });
}

export async function signalThrust(): Promise<void> {
  await postJson("/thrust", {});
}

export async function signalIdle(): Promise<void> {
  await postJson("/idle", {});
}
