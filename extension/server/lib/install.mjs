/**
 * install.mjs
 * ───────────
 * The Online Actions (ADR 0005), shared by the Managers tab and the agent
 * tools. Nothing else in Grease reaches the internet.
 *
 *   updateIndexes()          Developer: Managers › Update indexes
 *   installSupportedCore()   Developer (Managers, missing-core prompt) and agents (installCore)
 *   installRegistryLibrary() Developer (Managers) and agents (installLibrary)
 *   installGitHubLibrary()   Developer only, after a confirm dialog
 */

import { runCli, TIMEOUTS } from "./cli.mjs";
import { SUPPORTED_CORES, supportedCore, parseGitHubLibrary } from "./supported.mjs";

// Library Manager names: letters, digits, spaces, _ . - (never a leading "-",
// which arduino-cli would read as a flag).
const LIB_NAME = /^[A-Za-z0-9][A-Za-z0-9 _.\-]{0,62}$/;
const LIB_VERSION = /^[0-9A-Za-z][0-9A-Za-z.+\-]{0,31}$/;

const ALL_CORE_URLS = SUPPORTED_CORES.map((c) => c.url).filter(Boolean).join(",");

/** Online. Refresh the board index for every Supported Core, then the library index. */
export async function updateIndexes({ onData } = {}) {
  const core = await runCli(["core", "update-index", "--additional-urls", ALL_CORE_URLS], {
    timeoutMs: TIMEOUTS.index,
    onData,
  });
  const lib = await runCli(["lib", "update-index"], { timeoutMs: TIMEOUTS.index, onData });
  return { success: core.success && lib.success, core, lib };
}

/**
 * Online. Install one Supported Core. Its index URL is passed for this run
 * only; an empty value for built-in cores keeps any third-party URL in the
 * Developer's own config from being contacted.
 */
export async function installSupportedCore(coreId, { onData, signal } = {}) {
  const core = supportedCore(String(coreId ?? "").trim());
  if (!core) {
    return {
      success: false,
      exitCode: null,
      stdout: "",
      stderr:
        `"${coreId}" is not on Grease's Supported Core list.\n` +
        `Why: Grease installs only these cores: ${SUPPORTED_CORES.map((c) => c.id).join(", ")}.\n` +
        "What: pick one of them, or install other cores yourself with arduino-cli.",
    };
  }
  const urls = ["--additional-urls", core.url ?? ""];
  const index = await runCli(["core", "update-index", ...urls], { timeoutMs: TIMEOUTS.index, onData, signal });
  if (!index.success) return { ...index, core };
  const res = await runCli(["core", "install", core.id, ...urls], { timeoutMs: TIMEOUTS.coreInstall, onData, signal });
  return { ...res, core };
}

/** Online. Install a Library Manager library by its exact name, optionally pinned to a version. */
export async function installRegistryLibrary(name, version, { onData } = {}) {
  const n = String(name ?? "").trim();
  const v = version == null ? "" : String(version).trim();
  if (!LIB_NAME.test(n)) {
    return {
      success: false,
      exitCode: null,
      stdout: "",
      stderr: `"${n}" is not a Library Manager name.\nWhat: use the exact name shown by Managers › Library search, e.g. "Adafruit NeoPixel".`,
    };
  }
  if (v && !LIB_VERSION.test(v)) {
    return { success: false, exitCode: null, stdout: "", stderr: `"${v}" is not a library version, e.g. 1.12.3.` };
  }
  return runCli(["lib", "install", v ? `${n}@${v}` : n], { timeoutMs: TIMEOUTS.libInstall, onData });
}

/**
 * Online, Developer only. Clones a GitHub repository as a library. git
 * installs are allowed for this one run through the environment, never by
 * writing library.enable_unsafe_install to the global config.
 */
export async function installGitHubLibrary(input, { onData } = {}) {
  const parsed = parseGitHubLibrary(input);
  if (parsed.error) return { success: false, exitCode: null, stdout: "", stderr: parsed.error, parsed };
  const res = await runCli(["lib", "install", "--git-url", parsed.url], {
    timeoutMs: TIMEOUTS.libInstall,
    env: { ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL: "true" },
    onData,
  });
  return { ...res, parsed };
}
