/**
 * cliInstall.mjs
 * ──────────────
 * Installs arduino-cli into ~/.grease/bin when no copy is found, so a new
 * Developer has nothing to install by hand. A necessary install: it runs
 * without asking and the caller shows a pop-up saying so.
 *
 *   • one pinned version, every archive checked against its SHA-256 below;
 *   • unpacked with the system tar (bsdtar reads .zip on Windows 10+), no shell;
 *   • the same code on macOS, Windows and Linux.
 */

import { spawn } from "node:child_process";
import { createHash } from "node:crypto";
import fs from "node:fs";
import path from "node:path";

import { findOnPath, GREASE_BIN, resolveArduinoCli, runCli, TIMEOUTS } from "./cli.mjs";

const IS_WIN = process.platform === "win32";
const EXE = IS_WIN ? "arduino-cli.exe" : "arduino-cli";

/** Released 2026-06-05. The same checksums file is on downloads.arduino.cc and GitHub. */
export const CLI_VERSION = "1.5.1";

const BASE_URLS = [
  "https://downloads.arduino.cc/arduino-cli/",
  `https://github.com/arduino/arduino-cli/releases/download/v${CLI_VERSION}/`,
];

/** os key → archive name, size and SHA-256 (from 1.5.1-checksums.txt). */
export const ASSETS = Object.freeze({
  macOS_ARM64: { bytes: 17370016, sha256: "cb952e8c1621c95ef5f1d17831c945e3d0ec5973f89c557a7ec8feb9c4f7d4c9" },
  macOS_64bit: { bytes: 19355892, sha256: "c982e940027996bea9901050e95fae99c59c1dcfee54beedecaf28141e7bf2e7" },
  Windows_64bit: { bytes: 17965819, sha256: "fabe42e0eb04d00e776a66178299ff95a46c623dbc260f997e58fd514853dd40" },
  Windows_32bit: { bytes: 17468455, sha256: "885e491c7c7fb8b396151c09daa5c4c56d8b60697d172a5cfe72c939eed50fe3" },
  Linux_64bit: { bytes: 17630765, sha256: "28a8e119c498a25607821c36cb2dc49e8463941b261a0d99091baa7bc692dd2b" },
  Linux_ARM64: { bytes: 16155457, sha256: "1e69e077479f300614d4551334e0a33f08ee40b04315d83b8e7e0e94f0d0ee62" },
  Linux_ARMv7: { bytes: 16434757, sha256: "890af36e9873606e4dfa743534846186621b9f3a339175ea3ec481adecf07143" },
  Linux_ARMv6: { bytes: 16447595, sha256: "168aa0c632d7079fea0ccd30b3f1e928e89e2f59a339404f1d2f4a07ed6cc566" },
  Linux_32bit: { bytes: 17006509, sha256: "85ed48978e7553b16f187971f8202d380421b352696ab28327a36cc6d8f11c6a" },
});

function assetKey(platform = process.platform, arch = process.arch) {
  if (platform === "darwin") return arch === "arm64" ? "macOS_ARM64" : arch === "x64" ? "macOS_64bit" : null;
  // Windows on ARM has no build; it runs the x64 one under emulation.
  if (platform === "win32") return arch === "x64" || arch === "arm64" ? "Windows_64bit" : arch === "ia32" ? "Windows_32bit" : null;
  if (platform === "linux") {
    if (arch === "x64") return "Linux_64bit";
    if (arch === "arm64") return "Linux_ARM64";
    if (arch === "ia32") return "Linux_32bit";
    if (arch === "arm") return String(process.config?.variables?.arm_version) === "7" ? "Linux_ARMv7" : "Linux_ARMv6";
  }
  return null;
}

/** The archive for this computer: { name, bytes, sha256 }, or null when arduino-cli has no build for it. */
export function cliAsset(platform, arch) {
  const key = assetKey(platform, arch);
  if (!key) return null;
  const ext = key.startsWith("Windows") ? "zip" : "tar.gz";
  return { name: `arduino-cli_${CLI_VERSION}_${key}.${ext}`, ...ASSETS[key] };
}

function fail(error) {
  return { success: false, error };
}

/** SHA-256 of a file, as hex. */
export function sha256File(file) {
  return createHash("sha256").update(fs.readFileSync(file)).digest("hex");
}

/** Delete `file` unless its SHA-256 is `expected`. */
export function verifyDownload(file, expected) {
  if (sha256File(file) === expected) return true;
  fs.rmSync(file, { force: true });
  return false;
}

/** An AbortSignal that fires on `signal` or after `ms`. */
function withTimeout(signal, ms) {
  const timeout = AbortSignal.timeout(ms);
  if (!signal) return timeout;
  if (typeof AbortSignal.any === "function") return AbortSignal.any([signal, timeout]);
  const c = new AbortController();
  for (const s of [signal, timeout]) s.addEventListener("abort", () => c.abort(s.reason), { once: true });
  return c.signal;
}

async function download(url, file, total, onProgress, signal) {
  const res = await fetch(url, { signal: withTimeout(signal, TIMEOUTS.index), redirect: "follow" });
  if (!res.ok || !res.body) throw new Error(`HTTP ${res.status} from ${new URL(url).host}`);
  const out = fs.createWriteStream(file);
  let got = 0;
  try {
    for await (const chunk of res.body) {
      got += chunk.length;
      if (!out.write(chunk)) await new Promise((r) => out.once("drain", r));
      onProgress?.(Math.min(got / total, 1));
    }
  } finally {
    await new Promise((r) => out.end(r));
  }
}

function run(exe, args) {
  return new Promise((resolve) => {
    let err = "";
    let child;
    try {
      child = spawn(exe, args, { shell: false, windowsHide: true });
    } catch (e) {
      resolve(String(e?.message ?? e));
      return;
    }
    child.stderr?.on("data", (d) => (err += d));
    child.on("error", (e) => resolve(String(e?.message ?? e)));
    child.on("close", (code) => resolve(code === 0 ? null : err.trim() || `exit ${code}`));
  });
}

function tarPath() {
  if (IS_WIN) return path.join(process.env.SystemRoot || "C:\\Windows", "System32", "tar.exe");
  return findOnPath("tar") ?? "/usr/bin/tar";
}

function renameRetry(from, to) {
  for (let attempt = 0; ; attempt++) {
    try {
      fs.renameSync(from, to);
      return;
    } catch (e) {
      // Windows: antivirus scans a new .exe for a moment after it lands.
      if (IS_WIN && attempt < 10 && ["EPERM", "EBUSY", "EACCES"].includes(e?.code)) {
        Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, 200);
        continue;
      }
      throw e;
    }
  }
}

/**
 * Download, check and unpack arduino-cli into ~/.grease/bin.
 *
 * @param {{ onProgress?: (fraction: number) => void, signal?: AbortSignal }} [opts]
 * @returns {Promise<{ success: true, path: string, version: string, bytes: number } | { success: false, error: string, cancelled?: boolean }>}
 */
export async function installArduinoCli({ onProgress, signal } = {}) {
  const asset = cliAsset();
  if (!asset) return fail(`no arduino-cli build for this computer (${process.platform} ${process.arch})`);
  const tmp = path.join(GREASE_BIN, `.arduino-cli-${process.pid}-${Date.now()}`);
  const archive = path.join(tmp, asset.name);
  try {
    fs.mkdirSync(tmp, { recursive: true });
    let lastError = "";
    for (const base of BASE_URLS) {
      try {
        await download(base + asset.name, archive, asset.bytes, onProgress, signal);
        lastError = "";
        break;
      } catch (e) {
        if (signal?.aborted) return { ...fail("cancelled"), cancelled: true };
        lastError = String(e?.cause?.code ?? e?.message ?? e);
      }
    }
    if (lastError) return fail(`download failed (${lastError}): no internet, or blocked`);
    if (!verifyDownload(archive, asset.sha256)) return fail("download did not match its checksum");
    const tarError = await run(tarPath(), [IS_WIN ? "-xf" : "-xzf", archive, "-C", tmp]);
    if (tarError) return fail(`could not unpack the download (${tarError})`);
    const unpacked = path.join(tmp, EXE);
    if (!fs.existsSync(unpacked)) return fail(`the download has no ${EXE}`);
    if (!IS_WIN) fs.chmodSync(unpacked, 0o755);
    const dest = path.join(GREASE_BIN, EXE);
    renameRetry(unpacked, dest);
    const cli = resolveArduinoCli({ refresh: true });
    const v = await runCli(["version", "--json"], { timeoutMs: TIMEOUTS.quick });
    let version = "";
    try {
      version = JSON.parse(v.stdout).VersionString ?? "";
    } catch {
      /* checked below */
    }
    if (version !== CLI_VERSION) return fail(`the installed arduino-cli does not start (${v.stderr.trim() || "no version"})`);
    return { success: true, path: cli?.path ?? dest, version, bytes: asset.bytes };
  } catch (e) {
    return fail(String(e?.message ?? e));
  } finally {
    fs.rmSync(tmp, { recursive: true, force: true });
  }
}
