// Types for cliInstall.mjs, so the extension host (TypeScript) can import it.

export const CLI_VERSION: string;
export const ASSETS: Readonly<Record<string, { bytes: number; sha256: string }>>;
export function cliAsset(platform?: string, arch?: string): { name: string; bytes: number; sha256: string } | null;
export function sha256File(file: string): string;
export function verifyDownload(file: string, expected: string): boolean;
export function installArduinoCli(opts?: {
  onProgress?: (fraction: number) => void;
  signal?: AbortSignal;
}): Promise<
  | { success: true; path: string; version: string; bytes: number }
  | { success: false; error: string; cancelled?: boolean }
>;
