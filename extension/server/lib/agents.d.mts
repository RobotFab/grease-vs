// Types for agents.mjs, so the extension host (TypeScript) can import it.

export const SKILL_NAME: string;
export const BRIDGE_PATH: string;
export const AGENT_IDS: readonly string[];

export type SkillState = "current" | "outdated" | "edited" | "foreign" | "missing";

export interface SkillTarget {
  id: "claude" | "agents";
  dir: string;
  file: string;
  readers: string;
}

export interface SkillInstallResult extends SkillTarget {
  action: SkillState | "installed" | "updated" | "replaced" | "error";
  backup?: string | null;
  error?: string;
}

export interface AgentRow {
  id: string;
  name: string;
  invoke: string;
  present: boolean;
  skill: SkillState | "n/a";
  tools: "added" | "old" | "missing" | "unreadable" | "built-in" | "n/a";
  configFile: string | null;
  download: string;
}

export interface BridgeCommand {
  command: string;
  args: string[];
  env: Record<string, string>;
  via: "node" | "editor";
}

export function atomicWrite(file: string, text: string, mode?: number): void;
export function claudeDir(): string;
export function skillTargets(): SkillTarget[];
export function skillStatus(skillText: string): (SkillTarget & { state: SkillState })[];
export function installSkill(skillText: string, opts?: { force?: boolean }): SkillInstallResult[];
export function removeSkill(): { file: string; action: "removed" | "kept-edited" | "error"; error?: string }[];
export function installBridge(bridgeText: string, serverText: string): "current" | "installed" | "updated";
export function findNode(): string | null;
export function bridgeCommand(opts?: { editorNode?: string }): BridgeCommand | null;
export function findClaudeCli(): string | null;
export function detectAgents(opts?: { hostApp?: string; skillText?: string | null }): AgentRow[];
export function addTools(
  agentId: string,
  opts?: { editorNode?: string },
): Promise<{ ok: boolean; message: string; manual?: string }>;
