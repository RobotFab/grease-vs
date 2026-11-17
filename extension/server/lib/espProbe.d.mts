// Types for espProbe.mjs, so the extension host (TypeScript) can import it.

export interface ChipAnswer {
  /** "ESP32-C6", or null when the board did not answer. */
  chip: string | null;
  /** build.mcu, e.g. "esp32c6". */
  mcu: string | null;
  /** Generic FQBN for the chip, or null (C2, C61, unknown). */
  fqbn: string | null;
  at: number;
}

export function canProbe(usbId: string | null | undefined): boolean;
export function mcuOfChip(chip: string | null | undefined): string | null;
export function fqbnForChip(mcu: string | null | undefined, usbId: string | null | undefined): string | null;
export function parseChip(text: string): string | null;
export function cachedChip(board: { port: string; usbId: string; serialNumber?: string | null }): ChipAnswer | undefined;
export function forgetUnplugged(portsNow: Set<string>): void;
export function probeChip(
  board: { port: string; usbId: string; serialNumber?: string | null },
  busy?: (port: string) => boolean,
): Promise<ChipAnswer | null>;
export function mcuOfFqbn(fqbn: string | null | undefined): Promise<string | null>;
