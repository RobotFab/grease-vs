// Types for supported.mjs, so the extension host (TypeScript) can import it.

export interface SupportedCore {
  id: string;
  name: string;
  vendor: string;
  url: string | null;
  boards: string[];
}

export interface UsbBoard {
  core: string | null;
  /** The board's FQBN, or only a suggestion when ask is true. */
  fqbn: string | null;
  name: string;
  mode: "app" | "bootloader";
  ask: boolean;
}

export const SUPPORTED_CORES: readonly SupportedCore[];
export function supportedCore(id: string): SupportedCore | null;
export function additionalUrlsFor(coreId: string): string;
export function boardFromUsb(vid: string | null | undefined, pid: string | null | undefined): UsbBoard | null;
export function parseGitHubLibrary(
  input: string,
): { owner: string; repo: string; ref: string | null; url: string } | { error: string };
