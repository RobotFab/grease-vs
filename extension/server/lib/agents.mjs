/**
 * agents.mjs
 * ──────────
 * Puts Grease into the AI agents on this computer. Same code on macOS,
 * Windows and Linux:
 *
 *   • the Grease Skill (/grease) into the skill folders agents read:
 *     <Claude config>/skills for Claude Code, ~/.agents/skills for Codex,
 *     Cursor, VS Code Copilot, Windsurf and Devin (ADR 0006);
 *   • the stdio bridge into ~/.grease/bin/grease-mcp.mjs (ADR 0007);
 *   • one "grease" MCP entry per agent, only when the Developer asks.
 *
 * Every skill file Grease writes is recorded with its hash in
 * ~/.grease/extension/installed.json. Updates replace only Grease's own
 * unedited copies, and Remove deletes only what Grease wrote.
 */

import { spawn } from "node:child_process";
import crypto from "node:crypto";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";

import { GREASE_DIR, findOnPath } from "./cli.mjs";

const IS_WIN = process.platform === "win32";
const IS_MAC = process.platform === "darwin";
const HOME = os.homedir();
const MANIFEST = path.join(GREASE_DIR, "installed.json");

export const SKILL_NAME = "grease";
export const BRIDGE_PATH = path.join(HOME, ".grease", "bin", "grease-mcp.mjs");
/** AI Tether beside the bridge, so the bridge can start it with no editor and no other file. */
export const BRIDGE_SERVER_PATH = path.join(HOME, ".grease", "bin", "server.mjs");

// ── small file helpers ────────────────────────────────────────────────────

const sha = (text) => crypto.createHash("sha256").update(text).digest("hex");

function exists(p) {
  try {
    fs.statSync(p);
    return true;
  } catch {
    return false;
  }
}

function isFile(p) {
  try {
    return fs.statSync(p).isFile();
  } catch {
    return false;
  }
}

function readText(file) {
  try {
    return fs.readFileSync(file, "utf8");
  } catch {
    return null;
  }
}

function sleepSync(ms) {
  Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, ms);
}

/** Write through a temp file + rename, so a crash never leaves half a file. */
export function atomicWrite(file, text, mode) {
  fs.mkdirSync(path.dirname(file), { recursive: true });
  const tmp = `${file}.grease-${process.pid}.tmp`;
  fs.writeFileSync(tmp, text, mode === undefined ? undefined : { mode });
  for (let attempt = 0; ; attempt++) {
    try {
      fs.renameSync(tmp, file);
      return;
    } catch (e) {
      // Windows: antivirus or the agent itself can hold the file for a moment.
      if (IS_WIN && attempt < 4 && ["EPERM", "EBUSY", "EACCES"].includes(e?.code)) {
        sleepSync(60);
        continue;
      }
      try {
        fs.unlinkSync(tmp);
      } catch {
        /* already gone */
      }
      throw e;
    }
  }
}

/** Keep one copy of a config file as it was before Grease first changed it. */
function backupOnce(file) {
  const backup = `${file}.grease-backup`;
  if (!exists(backup) && isFile(file)) fs.copyFileSync(file, backup);
}

function readManifest() {
  try {
    const m = JSON.parse(fs.readFileSync(MANIFEST, "utf8"));
    return m && typeof m === "object" && !Array.isArray(m) ? m : {};
  } catch {
    return {};
  }
}

function writeManifest(m) {
  atomicWrite(MANIFEST, JSON.stringify(m, null, 2));
}

// ── where each agent keeps things ─────────────────────────────────────────

/** Claude Code's folder; CLAUDE_CONFIG_DIR moves it. */
export function claudeDir() {
  return process.env.CLAUDE_CONFIG_DIR || path.join(HOME, ".claude");
}

/** Claude Code keeps user-scope MCP servers in .claude.json, beside its folder. */
function claudeJson() {
  return process.env.CLAUDE_CONFIG_DIR
    ? path.join(process.env.CLAUDE_CONFIG_DIR, ".claude.json")
    : path.join(HOME, ".claude.json");
}

function codexDir() {
  return process.env.CODEX_HOME || path.join(HOME, ".codex");
}

function devinDir() {
  if (IS_WIN) return path.join(process.env.APPDATA || path.join(HOME, "AppData", "Roaming"), "devin");
  return path.join(process.env.XDG_CONFIG_HOME || path.join(HOME, ".config"), "devin");
}

/** Claude Desktop's config folder; there is no official Linux build. */
function claudeDesktopDir() {
  if (IS_MAC) return path.join(HOME, "Library", "Application Support", "Claude");
  if (IS_WIN) return path.join(process.env.APPDATA || path.join(HOME, "AppData", "Roaming"), "Claude");
  return null;
}

/**
 * Skill folders Grease fills. ~/.agents/skills is the cross-agent folder
 * (Codex, Cursor, VS Code Copilot, Windsurf, Devin), so it is always filled;
 * Claude Code reads only its own folder, filled when Claude Code is here.
 */
export function skillTargets() {
  const targets = [];
  if (exists(claudeDir())) {
    targets.push({ id: "claude", dir: path.join(claudeDir(), "skills", SKILL_NAME), readers: "Claude Code" });
  }
  targets.push({
    id: "agents",
    dir: path.join(HOME, ".agents", "skills", SKILL_NAME),
    readers: "Codex, Cursor, VS Code Copilot, Windsurf, Devin",
  });
  return targets.map((t) => ({ ...t, file: path.join(t.dir, "SKILL.md") }));
}

// ── the Grease Skill ──────────────────────────────────────────────────────

/**
 * State of each skill copy against `skillText`:
 * current | outdated (Grease's unedited copy) | edited (Grease's copy, changed
 * by the Developer) | foreign (a "grease" skill Grease did not write) | missing.
 */
export function skillStatus(skillText) {
  const records = readManifest().skills ?? {};
  const want = sha(skillText);
  return skillTargets().map((t) => {
    const current = readText(t.file);
    let state;
    if (current === null) state = "missing";
    else if (sha(current) === want) state = "current";
    else if (records[t.file]?.sha256 === sha(current)) state = "outdated";
    else state = records[t.file] ? "edited" : "foreign";
    return { ...t, state };
  });
}

/**
 * Install or update the skill everywhere it belongs. Without `force`, a copy
 * the Developer edited, or a "grease" skill Grease did not write, is kept.
 * With `force` (Reinstall skill), it is saved as SKILL.md.bak and replaced.
 */
export function installSkill(skillText, { force = false } = {}) {
  const manifest = readManifest();
  const records = manifest.skills ?? {};
  const want = sha(skillText);
  const results = [];
  for (const t of skillStatus(skillText)) {
    let action = t.state;
    let backup = null;
    try {
      if (t.state === "missing") {
        atomicWrite(t.file, skillText);
        action = "installed";
      } else if (t.state === "outdated") {
        atomicWrite(t.file, skillText);
        action = "updated";
      } else if (force && (t.state === "edited" || t.state === "foreign")) {
        backup = `${t.file}.bak`;
        fs.copyFileSync(t.file, backup);
        atomicWrite(t.file, skillText);
        action = "replaced";
      }
      if (["installed", "updated", "replaced", "current"].includes(action)) {
        records[t.file] = { sha256: want, at: new Date().toISOString() };
      }
    } catch (e) {
      action = "error";
      results.push({ ...t, action, error: e?.message ?? String(e) });
      continue;
    }
    results.push({ ...t, action, backup });
  }
  manifest.skills = records;
  writeManifest(manifest);
  return results;
}

/** Delete the skill copies Grease wrote. An edited copy is kept and reported. */
export function removeSkill() {
  const manifest = readManifest();
  const records = manifest.skills ?? {};
  const results = [];
  for (const [file, rec] of Object.entries(records)) {
    const current = readText(file);
    if (current === null) {
      delete records[file];
      continue;
    }
    if (sha(current) !== rec.sha256) {
      results.push({ file, action: "kept-edited" });
      continue;
    }
    try {
      fs.unlinkSync(file);
      delete records[file];
      try {
        fs.rmdirSync(path.dirname(file)); // only when empty
      } catch {
        /* the Developer put other files there */
      }
      results.push({ file, action: "removed" });
    } catch (e) {
      results.push({ file, action: "error", error: e?.message ?? String(e) });
    }
  }
  manifest.skills = records;
  writeManifest(manifest);
  return results;
}

// ── the stdio bridge ──────────────────────────────────────────────────────

/**
 * Copy the bridge and AI Tether to their fixed paths, so agent configs never
 * need rewriting and agents keep working when an editor's copy is replaced.
 */
export function installBridge(bridgeText, serverText) {
  const current = readText(BRIDGE_PATH);
  const currentServer = readText(BRIDGE_SERVER_PATH);
  if (current === bridgeText && currentServer === serverText) return "current";
  atomicWrite(BRIDGE_PATH, bridgeText, 0o755);
  atomicWrite(BRIDGE_SERVER_PATH, serverText, 0o644);
  return current === null ? "installed" : "updated";
}

/**
 * Node.js for agents to run the bridge with, as an absolute path: apps
 * started from the Dock or Start menu do not see the shell's PATH.
 */
export function findNode() {
  const onPath = findOnPath(IS_WIN ? "node.exe" : "node");
  if (onPath) return onPath;
  const candidates = IS_WIN
    ? [
        path.join(process.env.ProgramFiles || "C:\\Program Files", "nodejs", "node.exe"),
        path.join(process.env.LOCALAPPDATA || path.join(HOME, "AppData", "Local"), "Programs", "nodejs", "node.exe"),
        path.join(HOME, ".volta", "bin", "node.exe"),
      ]
    : [
        ...(IS_MAC ? ["/opt/homebrew/bin/node"] : []),
        "/usr/local/bin/node",
        "/usr/bin/node",
        path.join(HOME, ".volta", "bin", "node"),
      ];
  return candidates.find(isFile) ?? null;
}

/**
 * The command every agent registers. System Node.js when there is one;
 * otherwise the editor's own Electron in Node mode. Snap paths change on
 * every refresh, so a Snap editor never stands in for Node.js.
 */
export function bridgeCommand({ editorNode } = {}) {
  const node = findNode();
  if (node) return { command: node, args: [BRIDGE_PATH], env: {}, via: "node" };
  if (editorNode && !/[\\/]snap[\\/]/.test(editorNode)) {
    return { command: editorNode, args: [BRIDGE_PATH], env: { ELECTRON_RUN_AS_NODE: "1" }, via: "editor" };
  }
  return null;
}

/**
 * Claude Code installed only as an editor extension has no `claude` on PATH:
 * its binary sits in <home>/.<editor>/extensions/anthropic.claude-code-<ver>-<platform>/
 * resources/native-binary/. Every VS Code-derived editor (VS Code, Insiders,
 * Cursor, Windsurf, Antigravity, Kiro, VSCodium, …) keeps extensions in a
 * dot-folder of the home folder, so all of them are searched without a list.
 * The most recently installed copy wins.
 */
function findClaudeInEditors() {
  const exe = IS_WIN ? "claude.exe" : "claude";
  let homeEntries = [];
  try {
    homeEntries = fs.readdirSync(HOME).filter((d) => d.startsWith("."));
  } catch {
    return null;
  }
  const found = [];
  for (const dot of homeEntries) {
    const extDir = path.join(HOME, dot, "extensions");
    let exts;
    try {
      exts = fs.readdirSync(extDir);
    } catch {
      continue;
    }
    for (const name of exts) {
      if (!/^anthropic\.claude-code-/i.test(name)) continue;
      const p = path.join(extDir, name, "resources", "native-binary", exe);
      try {
        const st = fs.statSync(p);
        if (st.isFile()) found.push({ p, at: st.mtimeMs });
      } catch {
        /* not this copy */
      }
    }
  }
  found.sort((a, b) => b.at - a.at);
  return found[0]?.p ?? null;
}

/**
 * Claude Code's CLI as a real executable: PATH, the native installer's folders,
 * then the copy inside an editor's Claude Code extension. Windows npm `.cmd`
 * shims need a shell, so they are skipped.
 */
export function findClaudeCli() {
  const onPath = findOnPath(IS_WIN ? "claude.exe" : "claude");
  if (onPath) return onPath;
  const candidates = IS_WIN
    ? [path.join(HOME, ".local", "bin", "claude.exe")]
    : [
        path.join(HOME, ".local", "bin", "claude"),
        path.join(HOME, ".claude", "local", "claude"),
        "/opt/homebrew/bin/claude",
        "/usr/local/bin/claude",
      ];
  return candidates.find(isFile) ?? findClaudeInEditors();
}

// ── agents ────────────────────────────────────────────────────────────────

const CODEX_HEADER = /^\s*\[\s*mcp_servers\.(?:"grease"|'grease'|grease)\s*\]\s*(?:#.*)?$/m;

/** Agents Grease can reach. Presence comes from folders that exist; nothing is launched. */
function agentTable() {
  const desktop = claudeDesktopDir();
  return [
    {
      id: "claude-code",
      name: "Claude Code",
      invoke: "/grease",
      skill: "claude",
      kind: "claude-cli",
      file: claudeJson(),
      present: () => exists(claudeDir()) || !!findClaudeCli(),
      download: "https://claude.com/product/claude-code",
    },
    {
      id: "codex",
      name: "Codex",
      invoke: "$grease",
      skill: "agents",
      kind: "toml",
      file: path.join(codexDir(), "config.toml"),
      present: () => exists(codexDir()),
      download: "https://developers.openai.com/codex",
    },
    {
      id: "devin",
      name: "Devin",
      invoke: "/grease",
      skill: "agents",
      kind: "json",
      file: path.join(devinDir(), "mcp_config.json"),
      present: () => exists(devinDir()) || exists(path.join(HOME, ".devin")),
      download: "https://devin.ai",
    },
    {
      id: "cursor",
      name: "Cursor",
      invoke: "/grease",
      skill: "agents",
      kind: "json",
      file: path.join(HOME, ".cursor", "mcp.json"),
      present: () => exists(path.join(HOME, ".cursor")),
      download: "https://cursor.com/download",
    },
    {
      id: "windsurf",
      name: "Windsurf",
      invoke: "/grease",
      skill: "agents",
      kind: "json",
      file: path.join(HOME, ".codeium", "windsurf", "mcp_config.json"),
      present: () => exists(path.join(HOME, ".codeium", "windsurf")),
      download: "https://windsurf.com/download",
    },
    {
      id: "vscode",
      name: "VS Code Copilot",
      invoke: "/grease",
      skill: "agents",
      kind: "provider",
      file: null,
      present: () => false, // set from the host editor's name in detectAgents
      download: "https://code.visualstudio.com",
    },
    {
      id: "claude-desktop",
      name: "Claude Desktop",
      invoke: "ask in chat",
      skill: null,
      kind: "json",
      file: desktop ? path.join(desktop, "claude_desktop_config.json") : null,
      present: () => !!desktop && exists(desktop),
      download: "https://claude.ai/download",
    },
  ];
}

/** Agent ids the panel may send back. */
export const AGENT_IDS = Object.freeze(agentTable().map((a) => a.id));

/** added | old (1.0.13's HTTP entry with the key) | missing | unreadable */
function toolsStatus(agent) {
  if (agent.kind === "provider") return "built-in";
  if (!agent.file) return "n/a";
  const text = readText(agent.file);
  if (text === null || !text.trim()) return "missing";
  if (agent.kind === "toml") return CODEX_HEADER.test(text) ? "added" : "missing";
  try {
    const servers = JSON.parse(text)?.mcpServers ?? {};
    if (servers.grease) return "added";
    if (servers["arduino-grease"]) return "old";
    return "missing";
  } catch {
    return "unreadable";
  }
}

/**
 * One row per agent for the Help panel. `hostApp` is the editor's name
 * (vscode.env.appName); VS Code gets the tools through its own MCP API.
 */
export function detectAgents({ hostApp = "", skillText = null } = {}) {
  const skills = skillText ? skillStatus(skillText) : [];
  const isVsCode = /^visual studio code/i.test(hostApp);
  return agentTable().map((a) => {
    const present = a.id === "vscode" ? isVsCode : a.present();
    const copy = a.skill ? skills.find((s) => s.id === a.skill) : null;
    return {
      id: a.id,
      name: a.name,
      invoke: a.invoke,
      present,
      skill: a.skill ? (copy?.state ?? "missing") : "n/a",
      tools: present ? toolsStatus(a) : "n/a",
      configFile: a.file,
      download: a.download,
    };
  });
}

/** The same Claude Code command, for the Developer to paste when Grease cannot run it. */
function claudeAddArgs(cmd) {
  const env = Object.entries(cmd.env).flatMap(([k, v]) => ["-e", `${k}=${v}`]);
  return ["mcp", "add", SKILL_NAME, "-s", "user", ...env, "--", cmd.command, ...cmd.args];
}

function quoteForShell(s) {
  return /^[\w@%+=:,./\\-]+$/.test(s) ? s : `"${s.replace(/"/g, '\\"')}"`;
}

function runQuiet(exe, args, timeoutMs = 20_000) {
  // npm installs `claude` as a `#!/usr/bin/env node` script, which fails when
  // the editor was started from the Dock without node on PATH.
  const node = findNode();
  const env = node
    ? { ...process.env, PATH: [path.dirname(node), process.env.PATH ?? ""].join(path.delimiter) }
    : process.env;
  return new Promise((resolve) => {
    let out = "";
    let child;
    try {
      child = spawn(exe, args, { shell: false, windowsHide: true, env });
    } catch (e) {
      resolve({ code: -1, out: e?.message ?? String(e) });
      return;
    }
    const timer = setTimeout(() => child.kill(), timeoutMs);
    child.stdout?.on("data", (d) => (out += d));
    child.stderr?.on("data", (d) => (out += d));
    child.on("error", (e) => {
      clearTimeout(timer);
      resolve({ code: -1, out: e?.message ?? String(e) });
    });
    child.on("close", (code) => {
      clearTimeout(timer);
      resolve({ code, out });
    });
  });
}

function tomlBlock(cmd) {
  const lines = [
    "# Added by Grease VS: stdio bridge to AI Tether",
    "[mcp_servers.grease]",
    `command = ${JSON.stringify(cmd.command)}`,
    `args = [${cmd.args.map((a) => JSON.stringify(a)).join(", ")}]`,
  ];
  const env = Object.entries(cmd.env);
  if (env.length) lines.push(`env = { ${env.map(([k, v]) => `${k} = ${JSON.stringify(v)}`).join(", ")} }`);
  return lines.join("\n") + "\n";
}

/** Replace (or append) the [mcp_servers.grease] table, leaving every other line as it was. */
function writeCodexEntry(file, cmd) {
  const text = readText(file) ?? "";
  const kept = [];
  let inGrease = false;
  for (const line of text.split(/\r?\n/)) {
    const header = line.match(/^\s*\[\[?\s*([^\]]+?)\s*\]\]?\s*(?:#.*)?$/);
    if (header) {
      const name = header[1].replace(/["']/g, "");
      inGrease = name === "mcp_servers.grease" || name.startsWith("mcp_servers.grease.");
      // The marker comment belongs to the table it sits on.
      if (inGrease && /^# Added by (the Grease Extension|VS Grease|Grease VS)/.test(kept[kept.length - 1] ?? "")) kept.pop();
    }
    if (!inGrease) kept.push(line);
  }
  const body = kept.join("\n").replace(/\s+$/, "");
  if (text) backupOnce(file);
  atomicWrite(file, (body ? `${body}\n\n` : "") + tomlBlock(cmd));
}

/** Add or replace mcpServers.grease. A file that does not parse is never rewritten. */
function writeJsonEntry(file, cmd) {
  const text = readText(file);
  let cfg = {};
  if (text !== null && text.trim()) {
    try {
      cfg = JSON.parse(text);
    } catch {
      return { ok: false, message: `${file} is not valid JSON, so Grease left it alone.` };
    }
    if (!cfg || typeof cfg !== "object" || Array.isArray(cfg)) {
      return { ok: false, message: `${file} is not a JSON object, so Grease left it alone.` };
    }
  }
  if (!cfg.mcpServers || typeof cfg.mcpServers !== "object") cfg.mcpServers = {};
  // 1.0.13 wrote an HTTP entry holding the auth key; the bridge replaces it.
  const old = cfg.mcpServers["arduino-grease"];
  if (old && typeof old === "object" && old.headers && "x-grease-auth" in old.headers) {
    delete cfg.mcpServers["arduino-grease"];
  }
  const entry = { command: cmd.command, args: cmd.args };
  if (Object.keys(cmd.env).length) entry.env = cmd.env;
  cfg.mcpServers.grease = entry;
  if (text !== null) backupOnce(file);
  atomicWrite(file, JSON.stringify(cfg, null, 2) + "\n");
  return { ok: true };
}

/**
 * Register the bridge with one agent. Returns { ok, message, manual? };
 * `manual` is a command line the Developer can paste when Grease cannot
 * finish the job itself.
 */
export async function addTools(agentId, { editorNode } = {}) {
  const agent = agentTable().find((a) => a.id === agentId);
  if (!agent) return { ok: false, message: "Unknown agent." };
  if (agent.kind === "provider") {
    return { ok: true, message: `${agent.name} gets the Grease tools from the extension directly.` };
  }
  if (!agent.file) return { ok: false, message: `${agent.name} is not available on this OS.` };
  const cmd = bridgeCommand({ editorNode });
  if (!cmd) {
    return {
      ok: false,
      message: "Node.js was not found. Why: agents run the Grease bridge with Node.js. What: install Node.js LTS, restart the editor, then press Add tools again.",
    };
  }
  const restart = `Restart ${agent.name} to load the Grease tools.`;
  try {
    if (agent.kind === "claude-cli") {
      const cli = findClaudeCli();
      // The line to paste names the copy Grease found, since `claude` may not be on PATH.
      const manual = [cli ?? "claude", ...claudeAddArgs(cmd)].map(quoteForShell).join(" ");
      if (!cli) {
        return { ok: false, manual, message: "Claude Code's command was not found. Run this line in a terminal:" };
      }
      await runQuiet(cli, ["mcp", "remove", SKILL_NAME, "-s", "user"]);
      const r = await runQuiet(cli, claudeAddArgs(cmd));
      if (r.code !== 0) {
        return { ok: false, manual, message: `claude mcp add failed: ${r.out.trim().split("\n").pop()}. Run this line in a terminal:` };
      }
      return { ok: true, message: `Added. ${restart} Then type /grease.` };
    }
    if (agent.kind === "toml") {
      writeCodexEntry(agent.file, cmd);
      return { ok: true, message: `Added to ${agent.file}. ${restart} Then type $grease.` };
    }
    const r = writeJsonEntry(agent.file, cmd);
    if (!r.ok) return r;
    return { ok: true, message: `Added to ${agent.file}. ${restart}` };
  } catch (e) {
    return { ok: false, message: `Could not update ${agent.file}: ${e?.message ?? String(e)}` };
  }
}
