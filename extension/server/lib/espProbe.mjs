/**
 * espProbe.mjs
 * ────────────
 * Every ESP32 with built-in USB (C3, C6, H2, S3, P4) shows the same USB ID,
 * 303a:1001, and boards with a USB-serial chip (CH340, CP210x) say nothing
 * about the ESP32 behind it. The chip itself knows: esptool asks its ROM
 * bootloader and gets "ESP32-C6". That resets the board once (~1 s), so:
 *
 *   • only AI Tether probes (one process touches the port),
 *   • once per board: the answer is cached by USB serial number (the MAC on
 *     built-in USB), or by port + USB ID when the board has none,
 *   • never a port that is open, uploading, or already being probed.
 *
 * esptool comes with the ESP32 core (packages/esp32/tools/esptool_py), built
 * for each OS, so nothing extra is installed.
 */

import fs from "node:fs";
import path from "node:path";
import { spawn } from "node:child_process";
import { cliDataDir, killTree, runCli, parseCliJson, TIMEOUTS } from "./cli.mjs";

const IS_WIN = process.platform === "win32";

/** USB IDs that may hide an ESP32: Espressif built-in USB, and the USB-serial chips on ESP32 boards. */
const PROBE_IDS = new Set(["303a:1001", "303a:0002", "1a86:7523", "1a86:55d4", "10c4:ea60"]);

/** Chips with a generic FQBN in the ESP32 core (checked against esp32 3.3.x). */
const FAMILY_FQBN = {
  esp32: "esp32:esp32:esp32",
  esp32s2: "esp32:esp32:esp32s2",
  esp32s3: "esp32:esp32:esp32s3",
  esp32c3: "esp32:esp32:esp32c3",
  esp32c5: "esp32:esp32:esp32c5",
  esp32c6: "esp32:esp32:esp32c6",
  esp32h2: "esp32:esp32:esp32h2",
  esp32p4: "esp32:esp32:esp32p4",
};

const PROBE_TIMEOUT_MS = 15_000;

/**
 * The chip's FQBN with Serial on the USB port the board is plugged in by.
 * Without CDCOnBoot=cdc, Serial prints to the UART pins and a Super Mini
 * on 303a:1001 shows nothing. 303a:0002 is TinyUSB (S2, S3 in OTG mode).
 * A USB-serial chip (CH340, CP210x) carries the UART: no options.
 */
export function fqbnForChip(mcu, usbId) {
  const base = mcu ? FAMILY_FQBN[mcu] : null;
  if (!base) return null;
  const id = String(usbId ?? "").toLowerCase();
  if (id === "303a:1001") return `${base}:CDCOnBoot=cdc`;
  if (id === "303a:0002") return mcu === "esp32s3" ? `${base}:USBMode=default,CDCOnBoot=cdc` : `${base}:CDCOnBoot=cdc`;
  return base;
}

export function canProbe(usbId) {
  return PROBE_IDS.has(String(usbId ?? "").toLowerCase());
}

/** "ESP32-C6" → "esp32c6" (the ESP32 core's build.mcu). */
export function mcuOfChip(chip) {
  return chip ? String(chip).toLowerCase().replace(/[^a-z0-9]/g, "") : null;
}

/** Pull the chip name out of esptool 4.x or 5.x output. */
export function parseChip(text) {
  const m = String(text).match(/(?:Chip type:|Chip is|Connected to|Detecting chip type\.\.\.)\s*(ESP32(?:-[A-Z0-9]+)?)/i);
  return m ? m[1].toUpperCase() : null;
}

/** esptool from the newest esptool_py the ESP32 core installed, or null. */
async function findEsptool() {
  const data = await cliDataDir();
  if (!data) return null;
  const root = path.join(data, "packages", "esp32", "tools", "esptool_py");
  let versions;
  try {
    versions = fs.readdirSync(root);
  } catch {
    return null;
  }
  versions.sort((a, b) => b.localeCompare(a, undefined, { numeric: true }));
  for (const v of versions) {
    const exe = path.join(root, v, IS_WIN ? "esptool.exe" : "esptool");
    if (fs.existsSync(exe)) return exe;
  }
  return null;
}

function runEsptool(exe, port) {
  const args = ["--port", port, "--connect-attempts", "2", "--before", "default-reset", "--after", "hard-reset", "read-mac"];
  return new Promise((resolve) => {
    let out = "";
    let child;
    try {
      child = spawn(exe, args, { shell: false, windowsHide: true, detached: !IS_WIN });
    } catch (e) {
      resolve({ ok: false, out: String(e?.message ?? e) });
      return;
    }
    const timer = setTimeout(() => {
      killTree(child);
      resolve({ ok: false, out: `${out}\nesptool took more than ${PROBE_TIMEOUT_MS / 1000} s.` });
    }, PROBE_TIMEOUT_MS);
    child.stdout.on("data", (d) => (out += d));
    child.stderr.on("data", (d) => (out += d));
    child.on("error", (e) => {
      clearTimeout(timer);
      resolve({ ok: false, out: `${out}\n${e}` });
    });
    child.on("close", (code) => {
      clearTimeout(timer);
      resolve({ ok: code === 0, out });
    });
  });
}

const cache = new Map(); // key → { chip, mcu, fqbn, at } (chip null = not an ESP32, or no answer)
// An Espressif USB ID is surely an ESP32: a board that did not answer (crashed
// sketch, missed bootloader) is asked again after a minute. A USB-serial chip
// that did not answer is likely a Nano clone: it is not reset again until unplugged.
const RETRY_ESPRESSIF_MS = 60_000;
const inFlight = new Map(); // key → Promise
const keyOf = ({ port, usbId, serialNumber }) => (serialNumber ? `serial:${serialNumber}` : `port:${port}|${usbId}`);

/** Cached answer for this board, without probing. undefined = never probed. */
export function cachedChip(board) {
  return cache.get(keyOf(board));
}

/** Forget boards on ports that are gone, so plugging a different board into the same port probes again. */
export function forgetUnplugged(portsNow) {
  for (const k of cache.keys()) {
    if (k.startsWith("port:") && !portsNow.has(k.slice(5).split("|")[0])) cache.delete(k);
  }
}

/**
 * Read the chip on `port`. Returns { chip, mcu, fqbn } or null when the port
 * is not an ESP candidate, esptool is missing, or `busy(port)` says no.
 * A failed probe is cached as { chip: null } so a Nano clone is reset once only.
 */
export async function probeChip(board, busy = () => false) {
  if (!board?.port || !canProbe(board.usbId)) return null;
  const key = keyOf(board);
  const old = cache.get(key);
  if (old && (old.chip || !String(board.usbId).startsWith("303a:") || Date.now() - old.at < RETRY_ESPRESSIF_MS)) return old;
  if (inFlight.has(key)) return inFlight.get(key);
  if (busy(board.port)) return null;
  const job = (async () => {
    const exe = await findEsptool();
    if (!exe) return null; // ESP32 core not installed: try again after it is
    const r = await runEsptool(exe, board.port);
    const chip = parseChip(r.out);
    const mcu = mcuOfChip(chip);
    const answer = { chip, mcu, fqbn: fqbnForChip(mcu, board.usbId), at: Date.now() };
    cache.set(key, answer);
    return answer;
  })();
  inFlight.set(key, job);
  try {
    return await job;
  } finally {
    inFlight.delete(key);
  }
}

const mcuCache = new Map();

/** The chip an FQBN builds for (build.mcu), e.g. esp32:esp32:nano_nora → "esp32s3". Null if unknown. */
export async function mcuOfFqbn(fqbn) {
  if (!fqbn || !String(fqbn).startsWith("esp32:esp32:")) return null;
  const base = String(fqbn).split(":").slice(0, 3).join(":");
  if (mcuCache.has(base)) return mcuCache.get(base);
  const r = await runCli(["board", "details", "-b", base, "--format", "json"], { timeoutMs: TIMEOUTS.quick });
  const props = (r.success ? parseCliJson(`${r.stdout ?? ""}`) : null)?.build_properties ?? [];
  const line = props.find((l) => String(l).startsWith("build.mcu="));
  const mcu = line ? String(line).slice("build.mcu=".length).trim().toLowerCase() : null;
  if (r.success) mcuCache.set(base, mcu);
  return mcu;
}
