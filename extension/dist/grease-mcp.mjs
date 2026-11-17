#!/usr/bin/env node
/**
 * grease-mcp.mjs
 * ──────────────
 * The Grease stdio bridge (ADR 0007). Agents start this file as a stdio MCP
 * server; it forwards every JSON-RPC message to AI Tether (POST /mcp) and
 * writes the replies back on stdout, one message per line.
 *
 * Port and key are read from ~/.grease/extension/mcp-auth.json on every
 * request, so an agent's config never holds either and survives restarts.
 * When the recorded port does not answer, the bridge looks for AI Tether on
 * the ports it may use (3333–3392).
 *
 * One AI Tether per computer (ADR 0008): when none answers, the bridge starts
 * server.mjs as a detached process and waits for it. server.mjs always sits
 * next to this file: in ~/.grease/bin, the Claude Desktop bundle, or dist/.
 * Claude Desktop's built-in Node (an Electron utility process) cannot start a
 * second Node, so there, or when the detached start fails, the bridge runs
 * AI Tether in its own process; other agents share it while it lives.
 *
 * No dependencies: Node.js 18 or newer, or an editor's Electron with
 * ELECTRON_RUN_AS_NODE=1. Same file on macOS, Windows and Linux.
 */

import { spawn } from "node:child_process";
import fs from "node:fs";
import http from "node:http";
import os from "node:os";
import path from "node:path";
import readline from "node:readline";
import { fileURLToPath, pathToFileURL } from "node:url";
import util from "node:util";

const GREASE_EXT_DIR = path.join(os.homedir(), ".grease", "extension");
const AUTH_FILE = path.join(GREASE_EXT_DIR, "mcp-auth.json");
const TETHER_LOG = path.join(GREASE_EXT_DIR, "ai-tether.log");
// First start may download arduino-cli's indexes in the background; the
// port opens before that, so 15 s covers a slow disk.
const START_WAIT_MS = 15_000;
const FIRST_PORT = 3333;
const PORT_COUNT = 60;
// Longest AI Tether job is a board-core install (15 min); leave room above it.
const REQUEST_TIMEOUT_MS = 20 * 60_000;

const NOT_RUNNING =
  "AI Tether is not running. Why: the Grease bridge could not start it (see below). " +
  "What: reinstall Grease VS or the Grease .mcpb, then restart this agent.";

/** Build stamp of the server.mjs beside this file; scripts/build.mjs writes it. */
const BRIDGE_BUILD = Number("1794626889648");

let port = null;
let protocolVersion = null;

function readAuth() {
  try {
    const j = JSON.parse(fs.readFileSync(AUTH_FILE, "utf8"));
    return { key: typeof j.key === "string" ? j.key : null, port: Number(j.port) || null };
  } catch {
    return { key: null, port: null };
  }
}

function send(message) {
  process.stdout.write(JSON.stringify(message) + "\n");
}

function sendError(request, text) {
  if (request && request.id !== undefined && request.id !== null) {
    send({ jsonrpc: "2.0", id: request.id, error: { code: -32000, message: text } });
  }
}

function httpRequest(p, method, urlPath, headers, body, timeoutMs) {
  return new Promise((resolve, reject) => {
    const req = http.request({ host: "127.0.0.1", port: p, method, path: urlPath, headers, timeout: timeoutMs }, (res) => {
      const chunks = [];
      res.on("data", (c) => chunks.push(c));
      res.on("end", () => resolve({ status: res.statusCode ?? 0, headers: res.headers, text: Buffer.concat(chunks).toString("utf8") }));
      res.on("error", reject);
    });
    req.on("timeout", () => req.destroy(new Error("timed out")));
    req.on("error", reject);
    if (body) req.write(body);
    req.end();
  });
}

/** An AI Tether that accepts our key: /state answers 200 only with it. */
async function isTether(p, key) {
  try {
    const r = await httpRequest(p, "GET", "/state", { "x-grease-auth": key }, null, 1500);
    return r.status === 200 && JSON.parse(r.text)?.target !== undefined;
  } catch {
    return false;
  }
}

/** The recorded port first, then every port the extension may have picked, in parallel. */
async function findTether(recorded, key) {
  if (port && (await isTether(port, key))) return port;
  if (recorded && (await isTether(recorded, key))) return (port = recorded);
  const ports = Array.from({ length: PORT_COUNT }, (_, i) => FIRST_PORT + i).filter((p) => p !== recorded);
  const hits = await Promise.all(ports.map(async (p) => ((await isTether(p, key)) ? p : null)));
  port = hits.find((p) => p !== null) ?? null;
  return port;
}

/** server.mjs beside this file, or null when the install is incomplete. */
function serverPath() {
  const here = path.join(path.dirname(fileURLToPath(import.meta.url)), "server.mjs");
  return fs.existsSync(here) ? here : null;
}

/**
 * process.execPath can run server.mjs: plain Node, or an editor's Electron
 * with ELECTRON_RUN_AS_NODE=1. Claude Desktop's built-in Node is an Electron
 * utility process; it refuses to start its own binary as Node.
 */
function canStartNode() {
  return !process.versions.electron || process.env.ELECTRON_RUN_AS_NODE === "1";
}

let inProcess = null;
/**
 * Run AI Tether inside this process (once). Its log lines go to the log file:
 * stdout carries the agent's JSON-RPC and must hold nothing else.
 */
function startInProcess(server) {
  inProcess ??= (async () => {
    try {
      fs.mkdirSync(GREASE_EXT_DIR, { recursive: true });
    } catch {
      /* the stream's error handler below covers it */
    }
    const log = fs.createWriteStream(TETHER_LOG, { flags: "a" });
    log.on("error", () => {}); // a log that cannot be written must not end the bridge
    const toLog = (...args) => log.write(util.format(...args) + "\n");
    console.log = console.info = console.warn = console.error = console.debug = toLog;
    Object.assign(process.env, { GREASE_IN_PROCESS: "1", MCP_HOST: "127.0.0.1", MCP_IDE_NAME: "Grease bridge" });
    try {
      await import(pathToFileURL(server).href);
    } catch (e) {
      toLog(`[Grease] AI Tether could not run inside the bridge: ${e?.stack ?? e}`);
    }
  })();
  return inProcess;
}

/** Start AI Tether detached; null when the start failed at once. */
function spawnTether(server) {
  try {
    fs.mkdirSync(GREASE_EXT_DIR, { recursive: true });
    const log = fs.openSync(TETHER_LOG, "a");
    const child = spawn(process.execPath, [server], {
      cwd: os.homedir(),
      env: { ...process.env, MCP_HOST: "127.0.0.1", MCP_IDE_NAME: "Grease bridge" },
      detached: true,
      stdio: ["ignore", log, log],
      shell: false,
      windowsHide: true,
    });
    const run = { failed: false };
    // "error": refused or missing binary. "exit" before AI Tether answers: it
    // crashed, or it left because another one already runs (found below).
    child.once("error", () => (run.failed = true));
    child.once("exit", () => (run.failed = true));
    child.unref();
    fs.closeSync(log);
    return run;
  } catch {
    return null;
  }
}

let starting = null;
/**
 * Start AI Tether, then wait for it. Detached when this runtime can start
 * Node (it outlives this bridge); otherwise, or when that start fails, inside
 * this bridge. One start at a time.
 */
function startTether() {
  starting ??= (async () => {
    const server = serverPath();
    if (!server) return "no server.mjs beside the bridge";
    const ready = async () => {
      const auth = readAuth();
      return Boolean(auth.key && auth.port && (await isTether(auth.port, auth.key)));
    };
    const run = canStartNode() ? spawnTether(server) : null;
    if (!run) await startInProcess(server);
    for (let waited = 0; waited < START_WAIT_MS; waited += 250) {
      await new Promise((r) => setTimeout(r, 250));
      if (await ready()) return null;
      if (run?.failed && !inProcess) await startInProcess(server);
    }
    if (!inProcess) {
      await startInProcess(server);
      for (let waited = 0; waited < 5000; waited += 250) {
        await new Promise((r) => setTimeout(r, 250));
        if (await ready()) return null;
      }
    }
    return `it did not answer within ${START_WAIT_MS / 1000} s; see ${TETHER_LOG}`;
  })().finally(() => (starting = null));
  return starting;
}

let checkedBuild = false;
/**
 * A Tether from an older build lacks this bundle's tools, and it stays up
 * while any editor polls it. Replace it once per bridge, unless it is working
 * or holds a serial session, or runs inside another agent's bridge (it cannot
 * be stopped from here). Failing here only leaves the older Tether running.
 */
async function replaceOlderTether(p, key) {
  if (checkedBuild) return p;
  checkedBuild = true;
  try {
    const health = JSON.parse((await httpRequest(p, "GET", "/health", {}, null, 1500)).text);
    const state = JSON.parse((await httpRequest(p, "GET", "/state", { "x-grease-auth": key }, null, 1500)).text);
    if (!BRIDGE_BUILD || Number(health.build) >= BRIDGE_BUILD || health.busy || health.inProcess || state.serial?.isOpen || !serverPath()) return p;
    await httpRequest(p, "POST", "/shutdown", { "x-grease-auth": key }, null, 1500);
    for (let waited = 0; waited < 3000 && (await isTether(p, key)); waited += 150) {
      await new Promise((r) => setTimeout(r, 150));
    }
    port = null;
    return null;
  } catch {
    return p;
  }
}

function parseSse(text) {
  const out = [];
  for (const event of text.split(/\r?\n\r?\n/)) {
    const data = event
      .split(/\r?\n/)
      .filter((l) => l.startsWith("data:"))
      .map((l) => l.slice(5).replace(/^ /, ""))
      .join("\n");
    if (!data) continue;
    try {
      out.push(JSON.parse(data));
    } catch {
      /* keep-alive or partial event */
    }
  }
  return out;
}

async function forward(message) {
  let auth = readAuth();
  let p = auth.key ? await findTether(auth.port, auth.key) : null;
  if (p) p = await replaceOlderTether(p, auth.key);
  if (!p) {
    const why = await startTether();
    auth = readAuth();
    p = !why && auth.key ? await findTether(auth.port, auth.key) : null;
    if (!p) {
      sendError(message, `${NOT_RUNNING} Grease bridge: ${why ?? "AI Tether started but does not accept the key"}.`);
      return;
    }
  }
  const headers = {
    "content-type": "application/json",
    accept: "application/json, text/event-stream",
    "x-grease-auth": auth.key,
  };
  if (protocolVersion) headers["mcp-protocol-version"] = protocolVersion;
  let res;
  try {
    res = await httpRequest(p, "POST", "/mcp", headers, JSON.stringify(message), REQUEST_TIMEOUT_MS);
  } catch (e) {
    port = null;
    sendError(message, `AI Tether stopped answering (${e?.message ?? e}). ${NOT_RUNNING}`);
    return;
  }
  if (!res.text.trim()) {
    if (res.status >= 400) sendError(message, `AI Tether answered HTTP ${res.status}.`);
    return; // 202: a notification was accepted
  }
  const replies = String(res.headers["content-type"] ?? "").includes("text/event-stream")
    ? parseSse(res.text)
    : [JSON.parse(res.text)];
  for (const reply of replies.flat()) {
    if (message?.method === "initialize" && reply?.result?.protocolVersion) {
      protocolVersion = reply.result.protocolVersion;
    }
    send(reply);
  }
}

let pending = 0;
let closing = false;
const exitWhenDone = () => {
  if (closing && pending === 0) process.exit(0);
};

const rl = readline.createInterface({ input: process.stdin, crlfDelay: Infinity });
rl.on("line", (line) => {
  if (!line.trim()) return;
  let message;
  try {
    message = JSON.parse(line);
  } catch {
    send({ jsonrpc: "2.0", id: null, error: { code: -32700, message: "Parse error" } });
    return;
  }
  pending++;
  forward(message)
    .catch((e) => sendError(message, `Grease bridge error: ${e?.message ?? e}`))
    .finally(() => {
      pending--;
      exitWhenDone();
    });
});
// The agent closed our stdin: finish the replies already on their way (up to
// 3 s), then exit. A long tool call has no reader left, so it is not awaited.
rl.on("close", () => {
  closing = true;
  exitWhenDone();
  setTimeout(() => process.exit(0), 3000).unref();
});
