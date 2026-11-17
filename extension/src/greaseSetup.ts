/**
 * greaseSetup.ts
 * ──────────────
 * Checks and actions behind the Help panel, shared by the sidebar and by
 * activation: the arduino-cli and Node.js checks, the Grease Skill, the stdio
 * bridge, and adding the Grease tools to each agent.
 *
 * Every check runs a program by absolute path with no shell and a 5 s limit,
 * so it behaves the same on macOS, Windows and Linux.
 */

import { spawn } from "node:child_process";
import * as fs from "node:fs";
import * as path from "node:path";
import type * as vscode from "vscode";

import {
  findDownloadedCopies,
  firstRunMissing,
  installLine,
  pathFixLine,
  resolveArduinoCli,
  runCli,
} from "../server/lib/cli.mjs";
import {
  bridgeCommand,
  detectAgents,
  findNode,
  installBridge,
  installSkill,
  type AgentRow,
  type SkillInstallResult,
} from "../server/lib/agents.mjs";
import { SKILL_TEXT } from "../server/lib/skill.mjs";

const IS_WIN = process.platform === "win32";
const IS_MAC = process.platform === "darwin";

export interface CliCheck {
  state: "ok" | "offpath" | "missing";
  path?: string;
  version?: string;
  /** PATH fix line for terminals and agents (offpath only). */
  fix?: string;
  install: string;
  /** Copies in Downloads, offered but never used until the Developer picks one. */
  copies: string[];
  /** Installed by Grease into ~/.grease/bin (ADR 0012). */
  byGrease?: boolean;
  /** arduino-cli first-run parts still missing (board list, USB port finder...). */
  firstRun?: string[];
}

/** An install Grease runs by itself, shown in the Help panel while it runs or after it failed. */
export interface SetupJob {
  kind: "cli" | "firstRun" | "core";
  label: string;
  /** 0..1 while downloading, when the size is known. */
  fraction?: number;
  failed?: string;
}

export interface NodeCheck {
  /** editor: no Node.js, so agents run the bridge on the editor's own Electron. */
  state: "ok" | "editor" | "missing";
  path?: string;
  version?: string;
  install: string;
}

export function nodeInstallLine(): string {
  if (IS_WIN) return "winget install OpenJS.NodeJS.LTS";
  if (IS_MAC) return "brew install node";
  return "sudo apt install nodejs   (Fedora: sudo dnf install nodejs)";
}

/** First line a program prints for `args`, or null after 5 s or on failure. */
function firstLineOf(exe: string, args: string[]): Promise<string | null> {
  return new Promise((resolve) => {
    let out = "";
    let child: ReturnType<typeof spawn>;
    try {
      child = spawn(exe, args, { shell: false, windowsHide: true });
    } catch {
      resolve(null);
      return;
    }
    const timer = setTimeout(() => {
      child.kill();
      resolve(null);
    }, 5000);
    child.stdout?.on("data", (d) => (out += d));
    child.on("error", () => {
      clearTimeout(timer);
      resolve(null);
    });
    child.on("close", (code) => {
      clearTimeout(timer);
      resolve(code === 0 ? out.trim().split(/\r?\n/)[0] || null : null);
    });
  });
}

export async function checkArduinoCli(): Promise<CliCheck> {
  const cli = resolveArduinoCli({ refresh: true });
  if (!cli) return { state: "missing", install: installLine(), copies: findDownloadedCopies() };
  const r = await runCli(["version", "--json"], { timeoutMs: 5000 });
  let version: string | undefined;
  try {
    version = JSON.parse(r.stdout).VersionString;
  } catch {
    version = r.stdout.trim().split(/\r?\n/)[0] || undefined;
  }
  const firstRun = await firstRunMissing();
  const byGrease = cli.source === "grease";
  // Grease's own copy is not meant to be on PATH: agents reach it through AI Tether.
  if (cli.onPath || byGrease) return { state: "ok", path: cli.path, version, install: installLine(), copies: [], byGrease, firstRun };
  return {
    state: "offpath",
    path: cli.path,
    version,
    fix: pathFixLine(path.dirname(cli.path)),
    install: installLine(),
    copies: [],
    firstRun,
  };
}

export async function checkNode(editorNode: string): Promise<NodeCheck> {
  const node = findNode();
  if (node) {
    return { state: "ok", path: node, version: (await firstLineOf(node, ["--version"])) ?? undefined, install: nodeInstallLine() };
  }
  const cmd = bridgeCommand({ editorNode });
  if (cmd) return { state: "editor", path: cmd.command, install: nodeInstallLine() };
  return { state: "missing", install: nodeInstallLine() };
}

export function agentRows(hostApp: string): AgentRow[] {
  return detectAgents({ hostApp, skillText: SKILL_TEXT });
}

/**
 * Copy the bridge and install or update the skill; called on every
 * activation. `skill: false` (the Developer removed it) copies the bridge only.
 */
export function installGrease(
  context: vscode.ExtensionContext,
  output: vscode.OutputChannel,
  { skill = true }: { skill?: boolean } = {},
): { skills: SkillInstallResult[] } {
  try {
    const bridgeText = fs.readFileSync(context.asAbsolutePath(path.join("dist", "grease-mcp.mjs")), "utf8");
    const serverText = fs.readFileSync(context.asAbsolutePath(path.join("dist", "server.mjs")), "utf8");
    const bridge = installBridge(bridgeText, serverText);
    if (bridge !== "current") output.appendLine(`[Grease] Bridge ${bridge}: ~/.grease/bin/grease-mcp.mjs`);
  } catch (e) {
    output.appendLine(`[Grease] Bridge not copied: ${e instanceof Error ? e.message : String(e)}`);
  }
  let skills: SkillInstallResult[] = [];
  if (!skill) return { skills };
  try {
    skills = installSkill(SKILL_TEXT);
  } catch (e) {
    output.appendLine(`[Grease] Skill not installed: ${e instanceof Error ? e.message : String(e)}`);
  }
  for (const s of skills) {
    if (s.action === "current") continue;
    const note =
      s.action === "edited"
        ? "kept: you edited it (Reinstall skill replaces it)"
        : s.action === "foreign"
          ? "kept: another skill named grease is there"
          : s.action === "error"
            ? `failed: ${s.error}`
            : s.action;
    output.appendLine(`[Grease] Skill ${note}: ${s.file}`);
  }
  return { skills };
}
