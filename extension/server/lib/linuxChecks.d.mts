// Types for linuxChecks.mjs, so the extension host (TypeScript) can import it.

export interface LinuxCheck {
  id: string;
  blocking: boolean;
  title: string;
  why: string;
  fix?: string;
  note?: string;
}

export function linuxChecks(): Promise<LinuxCheck[]>;
