import path from "node:path";
import { cliDataDir, runCli, TIMEOUTS, parseCliJson, explainCliFailure } from "./cli.mjs";
import { boardFromUsb, supportedCore } from "./supported.mjs";
import { canProbe, probeChip, cachedChip, forgetUnplugged } from "./espProbe.mjs";

function normalizeSketchPath(sketchPath) {
  const resolved = path.resolve(String(sketchPath || ""));
  if (path.extname(resolved).toLowerCase() === ".ino") {
    return path.dirname(resolved);
  }
  return resolved;
}

/**
 * @returns {Promise<{ success: boolean, boardsJson: any, raw: string, stderr: string }>}
 */
export async function detectBoards() {
  const result = await runCli(["board", "list", "--format", "json"], { timeoutMs: TIMEOUTS.boardList });
  const raw = `${result.stdout ?? ""}`;
  const stderr = `${result.stderr ?? ""}`;
  if (!result.success) return { success: false, boardsJson: null, raw, stderr };
  const boardsJson = parseCliJson(raw);
  if (!boardsJson) return { success: false, boardsJson: null, raw, stderr: `${stderr}\nFailed to parse board list JSON.` };
  return { success: true, boardsJson, raw, stderr };
}

/**
 * One row per USB port, for agents: the board's name, the FQBN Grease
 * suggests, and what to do next. arduino-cli names a board only when its core
 * is installed; Grease's offline USB map names it either way, so a board whose
 * core is missing still says which core to install. Ports without a USB
 * identity (debug-console, Bluetooth) are left out.
 */
export async function summarizeBoards(boardsJson, { busy = () => false } = {}) {
  const usb = (boardsJson?.detected_ports ?? []).filter((d) => d.port?.properties?.vid);
  forgetUnplugged(new Set(usb.map((d) => d.port.address)));
  if (!usb.length) return [];
  const cores = await runCli(["core", "list", "--format", "json"], { timeoutMs: TIMEOUTS.quick });
  const installed = new Set(
    ((cores.success ? parseCliJson(`${cores.stdout ?? ""}`) : null)?.platforms ?? []).map((p) => p.id),
  );
  const rows = [];
  for (const d of usb) {
    const { vid, pid, product, serialNumber } = d.port.properties;
    const port = d.port.address;
    const usbId = `${String(vid).replace(/^0x/i, "").toLowerCase()}:${String(pid).replace(/^0x/i, "").toLowerCase()}`;
    const cli = d.matching_boards ?? [];
    const map = boardFromUsb(vid, pid);
    if (cli.length === 1) {
      rows.push({ port, usbId, name: cli[0].name, fqbn: cli[0].fqbn, coreInstalled: true });
      continue;
    }
    if (map && map.core && !installed.has(map.core)) {
      const core = supportedCore(map.core);
      rows.push({
        port, usbId, name: map.name, fqbn: map.fqbn, coreInstalled: false, core: map.core,
        next: `Its core ${map.core} (${core?.name ?? map.core}) is not installed. Tell the Developer, then call installCore with "${map.core}". It downloads 100 MB to 1 GB.`,
      });
      continue;
    }
    if (map && !map.ask && map.fqbn) {
      rows.push({ port, usbId, name: map.name, fqbn: map.fqbn, coreInstalled: true, ...(map.mode === "bootloader" ? { next: "The board is in its bootloader. Upload still works; serial starts after the upload." } : {}) });
      continue;
    }
    // One USB ID for many ESP32 chips: ask the chip itself (resets the board once).
    if (canProbe(usbId) && installed.has("esp32:esp32")) {
      if (busy(port) && !cachedChip({ port, usbId, serialNumber })?.chip) {
        rows.push({
          port, usbId, name: map?.name ?? product ?? "USB board", fqbn: null, coreInstalled: true,
          next: "Grease reads which ESP32 chip this is only while the port is free (it resets the board once). Call serialClose, then detectBoards again.",
        });
        continue;
      }
      const probe = await probeChip({ port, usbId, serialNumber }, busy);
      if (probe?.chip) {
        rows.push({
          port, usbId, name: `${probe.chip} (chip read over USB)`, fqbn: probe.fqbn, coreInstalled: true, chip: probe.chip,
          next: probe.fqbn
            ? `Grease read the chip. ${probe.fqbn} works for any ${probe.chip} board; if the Developer names the exact board (XIAO, Super Mini, DevKit), listBoards "${probe.chip}" may list a closer FQBN.`
            : `Grease read the chip (${probe.chip}). Find its FQBN with listBoards "${probe.chip}", then setTarget.`,
        });
        continue;
      }
      if (probe && String(usbId).startsWith("303a:")) {
        rows.push({
          port, usbId, name: map?.name ?? "ESP32 with built-in USB", fqbn: null, coreInstalled: true,
          next: "The ESP32 did not answer (a crashed or sleeping sketch can block it). Ask the Developer to hold BOOT, tap RESET, release BOOT, then call detectBoards again.",
        });
        continue;
      }
    }
    const options = cli.map((b) => b.fqbn);
    rows.push({
      port, usbId, name: map?.name ?? product ?? "Unknown USB board", fqbn: map?.fqbn ?? null,
      coreInstalled: map?.core ? installed.has(map.core) : null,
      ...(options.length > 1 ? { options } : {}),
      next: "Ask the Developer which board this is, find its FQBN with listBoards, then setTarget.",
    });
  }
  return rows;
}

const MAX_BOARDS_LISTED = 50;
const squash = (text) => String(text).toLowerCase().replace(/[^a-z0-9]/g, "");

/**
 * Installed board cores with their folders, and the boards they define.
 * `query` keeps boards whose name or FQBN holds every word, ignoring case and
 * punctuation, so "xiao c6" finds XIAO_ESP32C6.
 */
export async function listBoards(query = "") {
  const [cores, boards, dataDir] = await Promise.all([
    runCli(["core", "list", "--format", "json"], { timeoutMs: TIMEOUTS.quick }),
    runCli(["board", "listall", "--format", "json"], { timeoutMs: TIMEOUTS.boardList }),
    cliDataDir(),
  ]);
  const coresJson = cores.success ? parseCliJson(`${cores.stdout ?? ""}`) : null;
  const boardsJson = boards.success ? parseCliJson(`${boards.stdout ?? ""}`) : null;
  if (!coresJson || !boardsJson) {
    return explainCliFailure({ ...(cores.success ? boards : cores), success: false });
  }
  const words = String(query).split(/\s+/).map(squash).filter(Boolean);
  const matches = (boardsJson.boards ?? [])
    .map((b) => ({ name: b.name, fqbn: b.fqbn }))
    .filter((b) => words.every((w) => squash(`${b.name} ${b.fqbn}`).includes(w)));
  return {
    success: true,
    cores: (coresJson.platforms ?? []).map((p) => {
      const [vendor] = String(p.id).split(":");
      return {
        id: p.id,
        installed: p.installed_version ?? null,
        latest: p.latest_version ?? null,
        folder: dataDir ? path.join(dataDir, "packages", vendor) : null,
      };
    }),
    boards: matches.slice(0, MAX_BOARDS_LISTED),
    boardCount: matches.length,
  };
}

const VERBOSE_TAIL_CHARS = 6000;

/**
 * A failing build hook prints nothing without --verbose, only "exit status 1".
 * Build again with --verbose and put the end of the log in stderr, so the
 * Developer and the agent see the command that failed.
 */
async function withBuildLog(result, fqbn, sketchDir) {
  if (result.success || result.exitCode === null) return result;
  const detail = `${result.stderr ?? ""}`.replace(/Error during build: exit status \d+/g, "").trim();
  if (detail) return result;
  const verbose = await runCli(["compile", "--verbose", "--fqbn", fqbn, sketchDir], { timeoutMs: TIMEOUTS.compile });
  const log = `${verbose.stdout ?? ""}\n${verbose.stderr ?? ""}`.trim().slice(-VERBOSE_TAIL_CHARS);
  return log ? { ...result, stderr: `${result.stderr ?? ""}\n${log}`.trim() } : result;
}

/**
 * @param {{ fqbn: string, sketchPath: string }} args
 */
export async function compileSketch({ fqbn, sketchPath }) {
  const sketchDir = normalizeSketchPath(sketchPath);
  const result = await runCli(["compile", "--fqbn", fqbn, sketchDir], { timeoutMs: TIMEOUTS.compile });
  return explainCliFailure(await withBuildLog(result, fqbn, sketchDir));
}

/**
 * @param {{ fqbn: string, port: string, sketchPath: string }} args
 */
export async function uploadSketch({ fqbn, port, sketchPath }) {
  return explainCliFailure(
    await runCli(["upload", "-p", port, "--fqbn", fqbn, normalizeSketchPath(sketchPath)], { timeoutMs: TIMEOUTS.upload }),
  );
}

/**
 * Compile, then upload, in one arduino-cli run. Agents use this: a bare
 * `upload` flashes whatever the last build left, which is the old sketch
 * when the agent edited the file but skipped the compile.
 * @param {{ fqbn: string, port: string, sketchPath: string }} args
 */
export async function compileAndUploadSketch({ fqbn, port, sketchPath }) {
  const sketchDir = normalizeSketchPath(sketchPath);
  const result = await runCli(["compile", "--upload", "-p", port, "--fqbn", fqbn, sketchDir], {
    timeoutMs: TIMEOUTS.compile + TIMEOUTS.upload,
  });
  return explainCliFailure(await withBuildLog(result, fqbn, sketchDir));
}
