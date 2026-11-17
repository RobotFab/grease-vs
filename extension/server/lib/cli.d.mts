// Types for cli.mjs, so the extension host (TypeScript) can import it.

export interface CliResult {
  success: boolean;
  exitCode: number | null;
  stdout: string;
  stderr: string;
  timedOut?: boolean;
  notFound?: boolean;
  cancelled?: boolean;
  /** Set when arduino-cli's first-run download failed, so the command never ran. */
  firstRun?: boolean;
}

export interface ResolvedCli {
  path: string;
  source: "env" | "saved" | "PATH" | "found" | "grease";
  onPath: boolean;
}

export const GREASE_DIR: string;
export const GREASE_BIN: string;
export const OFFLINE_ENV: Readonly<Record<string, string>>;
export const TIMEOUTS: Readonly<{
  quick: number;
  boardList: number;
  compile: number;
  upload: number;
  libInstall: number;
  index: number;
  coreInstall: number;
}>;

export function readGreaseSettings(): Record<string, unknown>;
export function writeGreaseSettings(patch: Record<string, unknown>): Record<string, unknown>;
export function findOnPath(name: string): string | null;
export function findDownloadedCopies(): string[];
export function resolveArduinoCli(opts?: { refresh?: boolean }): ResolvedCli | null;
export function installLine(): string;
export function pathFixLine(dir: string): string;
export function notFoundMessage(): string;
export function killTree(child: unknown): void;
export function runCli(
  args: string[],
  opts?: {
    cwd?: string;
    timeoutMs?: number;
    env?: Record<string, string>;
    onData?: (s: string) => void;
    signal?: AbortSignal;
    skipFirstRun?: boolean;
  },
): Promise<CliResult>;

export interface FirstRunEvent {
  phase: "start" | "done" | "failed";
  missing?: string[];
  message?: string;
}
/** arduino-cli's data folder (Arduino15), or null when arduino-cli is missing. */
export function cliDataDir(): Promise<string | null>;
export function firstRunMissing(): Promise<string[]>;
export function onFirstRun(fn: (e: FirstRunEvent) => void): () => void;
export function ensureFirstRun(opts?: { force?: boolean }): Promise<{ success: true; ran: boolean } | (CliResult & { success: false })>;
export function firstRunFailure(): (CliResult & { success: false }) | null;
export function explainCliFailure<T extends CliResult>(result: T): T;
export function parseCliJson(text: string): any;
