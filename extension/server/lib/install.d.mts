// Types for install.mjs, so the extension host (TypeScript) can import it.

import type { CliResult } from "./cli.mjs";
import type { SupportedCore } from "./supported.mjs";

type OnData = { onData?: (s: string) => void; signal?: AbortSignal };

export function updateIndexes(opts?: OnData): Promise<{ success: boolean; core: CliResult; lib: CliResult }>;
export function installSupportedCore(coreId: string, opts?: OnData): Promise<CliResult & { core?: SupportedCore }>;
export function installRegistryLibrary(name: string, version?: string | null, opts?: OnData): Promise<CliResult>;
export function installGitHubLibrary(
  input: string,
  opts?: OnData,
): Promise<CliResult & { parsed: { url?: string; error?: string } }>;
