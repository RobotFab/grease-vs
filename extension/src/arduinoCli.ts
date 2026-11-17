/**
 * arduinoCli.ts
 * ─────────────
 * Extension-side wrappers around arduino-cli. All spawning goes through
 * server/lib/cli.mjs, the same runner AI Tether uses: absolute path, no
 * shell, the offline environment, per-operation timeouts, tree kill.
 *
 * No VS Code APIs are imported here on purpose — keeping this module
 * UI-agnostic makes it easy to unit-test and reuse.
 */

import { runCli, TIMEOUTS, parseCliJson, explainCliFailure, type CliResult } from "../server/lib/cli.mjs";
import { boardFromUsb } from "../server/lib/supported.mjs";

export type { CliResult };
export { TIMEOUTS, explainCliFailure };

/**
 * Run arduino-cli with the given args. Offline unless the args themselves
 * download (installs and index updates live in server/lib/install.mjs).
 */
export async function runArduinoCli(
  args: string[],
  cwd?: string,
  timeoutMs: number = TIMEOUTS.boardList,
): Promise<CliResult> {
  return runCli(args, { cwd, timeoutMs });
}

/**
 * An FQBN looks like `vendor:arch:board[:menu=value...]`. The "core package"
 * is the first two segments — that's what gets installed via `core install`.
 * Example: `esp32:esp32:XIAO_ESP32C6` → `esp32:esp32`.
 */
export function extractCoreFromFqbn(fqbn: string | null | undefined): string | null {
  if (!fqbn || typeof fqbn !== "string") return null;
  const parts = fqbn.split(":");
  if (parts.length >= 2) return `${parts[0]}:${parts[1]}`;
  return null;
}

/** Return true if the given core package is already present in `core list`. */
export async function isCoreInstalled(corePkg: string): Promise<boolean> {
  const result = await runCli(["core", "list", "--json"], { timeoutMs: TIMEOUTS.quick });
  if (!result.success) return false;
  const platforms = parseCliJson(result.stdout)?.platforms;
  return Array.isArray(platforms) && platforms.some((p: any) => String(p?.id ?? "") === corePkg);
}

/** A single board+port discovery row from `arduino-cli board list --format json`. */
export interface BoardCandidate {
  port: string;
  protocol?: string;
  fqbn?: string;
  name?: string;
  vid?: string | null;
  pid?: string | null;
  serialNumber?: string | null;
  /** Set when the FQBN came from Grease's offline USB map, not from an installed core. */
  fromUsbMap?: boolean;
  /** Core the board needs, from the offline USB map (may not be installed yet). */
  neededCore?: string;
}

export interface BoardDetection {
  success: boolean;
  candidates: BoardCandidate[];
  raw: string;
  stderr: string;
}

/**
 * Ask arduino-cli to enumerate connected boards. The CLI returns a tree where
 * each detected port may have zero, one, or many "matching boards" (USB VID/PID
 * matches against INSTALLED cores). We flatten that into one candidate per
 * (port × board) pair so the UI can show the user a single picker list.
 *
 * A port with no match is looked up in Grease's offline USB map (Pololu Romi
 * 32U4, boards whose core is not installed yet). The cloud lookup arduino-cli
 * would otherwise make is switched off by the offline environment.
 */
export async function detectBoardCandidates(): Promise<BoardDetection> {
  const result = await runCli(["board", "list", "--format", "json"], { timeoutMs: TIMEOUTS.boardList });
  const raw = result.stdout;
  const stderr = result.stderr;
  if (!result.success) return { success: false, candidates: [], raw, stderr };
  const parsed = parseCliJson(raw);
  if (!parsed) return { success: false, candidates: [], raw, stderr: `${stderr}\nFailed to parse board list JSON.` };
  const ports = Array.isArray(parsed?.detected_ports) ? parsed.detected_ports : [];
  const candidates: BoardCandidate[] = [];
  for (const p of ports) {
    const address = p?.port?.address;
    if (typeof address !== "string") continue;
    const protocol = p?.port?.protocol;
    const vid = p?.port?.properties?.vid ?? null;
    const pid = p?.port?.properties?.pid ?? null;
    const serialNumber = p?.port?.properties?.serialNumber ?? null;
    const matching = Array.isArray(p?.matching_boards) ? p.matching_boards : [];
    if (matching.length === 0) {
      const known = boardFromUsb(vid, pid);
      candidates.push(
        known && known.fqbn && !known.ask
          ? { port: address, protocol, vid, pid, serialNumber, fqbn: known.fqbn, name: known.name, fromUsbMap: true, neededCore: known.core }
          : { port: address, protocol, vid, pid, serialNumber, name: known?.name, neededCore: known?.core ?? undefined },
      );
      continue;
    }
    for (const b of matching) {
      candidates.push({ port: address, protocol, fqbn: b?.fqbn, name: b?.name, vid, pid, serialNumber });
    }
  }
  return { success: true, candidates, raw, stderr };
}
