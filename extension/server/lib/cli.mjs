/**
 * cli.mjs
 * ───────
 * The one place Grease runs arduino-cli. AI Tether (server.mjs) imports it
 * directly; the extension host imports it through src/arduinoCli.ts (esbuild
 * bundles it into both). Same code on macOS, Windows and Linux:
 *
 *   • resolveArduinoCli() finds the binary by absolute path, without a shell.
 *   • runCli() spawns it with shell:false, the offline env (ADR 0005), a
 *     per-operation timeout, and kills the whole process tree on timeout.
 *   • explainCliFailure() turns raw arduino-cli errors into WHY + WHAT.
 */

import { spawn } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";

const IS_WIN = process.platform === "win32";
const IS_MAC = process.platform === "darwin";
const EXE = IS_WIN ? "arduino-cli.exe" : "arduino-cli";

/** Grease-owned settings, readable by AI Tether whoever started it (VS Code, the bridge, Claude Desktop). */
export const GREASE_DIR = path.join(os.homedir(), ".grease", "extension");
/** Where Grease puts the arduino-cli it installs itself (cliInstall.mjs), next to the bridge. */
export const GREASE_BIN = path.join(os.homedir(), ".grease", "bin");
const SETTINGS_FILE = path.join(GREASE_DIR, "settings.json");

/**
 * Every arduino-cli run gets these. They keep a run offline unless the caller
 * marks it online: no cloud VID/PID lookups during `board list`, no update
 * check, no git/zip library installs. Never written to the global config.
 */
export const OFFLINE_ENV = Object.freeze({
  ARDUINO_NETWORK_CLOUD_API_SKIP_BOARD_DETECTION_CALLS: "true",
  ARDUINO_UPDATER_ENABLE_NOTIFICATION: "false",
  ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL: "false",
});

/** Per-operation limits, replacing the single 30 s kill. */
export const TIMEOUTS = Object.freeze({
  quick: 15_000, // version, config dump, core list, lib list
  boardList: 30_000,
  compile: 300_000, // cold ESP32 builds take 25–60 s, big ones more
  upload: 120_000,
  libInstall: 300_000,
  index: 300_000,
  coreInstall: 900_000, // esp32 is several hundred MB on a slow link
});

// ── settings ──────────────────────────────────────────────────────────────

export function readGreaseSettings() {
  try {
    return JSON.parse(fs.readFileSync(SETTINGS_FILE, "utf8")) ?? {};
  } catch {
    return {};
  }
}

export function writeGreaseSettings(patch) {
  const next = { ...readGreaseSettings(), ...patch };
  fs.mkdirSync(GREASE_DIR, { recursive: true });
  const tmp = `${SETTINGS_FILE}.${process.pid}.tmp`;
  fs.writeFileSync(tmp, JSON.stringify(next, null, 2));
  fs.renameSync(tmp, SETTINGS_FILE);
  return next;
}

// ── resolving the binary ──────────────────────────────────────────────────

function isExecutableFile(p) {
  try {
    if (!fs.statSync(p).isFile()) return false;
    if (!IS_WIN) fs.accessSync(p, fs.constants.X_OK);
    return true;
  } catch {
    return false;
  }
}

function pathDirs() {
  return (process.env.PATH ?? "")
    .split(path.delimiter)
    .map((d) => d.trim().replace(/^"(.*)"$/, "$1"))
    .filter(Boolean);
}

/** First `name` on PATH, as an absolute path, or null. No shell, no `which`. */
export function findOnPath(name) {
  for (const dir of pathDirs()) {
    const candidate = path.join(dir, name);
    if (isExecutableFile(candidate)) return candidate;
  }
  return null;
}

/** Install locations that are safe to use automatically, per OS. */
function trustedDirs() {
  const home = os.homedir();
  const dirs = [path.join(home, ".local", "bin"), path.join(home, "bin")];
  // Arduino IDE 2 ships its own arduino-cli inside the app.
  const ideBackend = ["resources", "app", "lib", "backend", "resources"];
  if (IS_MAC) {
    const macBackend = ["Contents", "Resources", "app", "lib", "backend", "resources"];
    dirs.push(
      "/opt/homebrew/bin",
      "/usr/local/bin",
      path.join("/Applications", "Arduino IDE.app", ...macBackend),
      path.join(home, "Applications", "Arduino IDE.app", ...macBackend),
    );
  } else if (IS_WIN) {
    const local = process.env.LOCALAPPDATA || path.join(home, "AppData", "Local");
    const programFiles = process.env.ProgramFiles || "C:\\Program Files";
    const programData = process.env.ProgramData || "C:\\ProgramData";
    dirs.push(
      path.join(local, "Microsoft", "WinGet", "Links"),
      path.join(programFiles, "Arduino CLI"),
      path.join(local, "Programs", "Arduino IDE", ...ideBackend),
      path.join(programFiles, "Arduino IDE", ...ideBackend),
      path.join(home, "scoop", "shims"),
      path.join(programData, "chocolatey", "bin"),
    );
    // winget unpacks portable zips under …\WinGet\Packages\<id>_<source>\
    const pkgs = path.join(local, "Microsoft", "WinGet", "Packages");
    try {
      for (const d of fs.readdirSync(pkgs)) {
        if (/^ArduinoSA\./i.test(d)) dirs.push(path.join(pkgs, d));
      }
    } catch {
      /* no winget packages folder */
    }
  } else {
    dirs.push("/usr/local/bin", "/usr/bin", "/snap/bin", "/home/linuxbrew/.linuxbrew/bin");
  }
  return dirs;
}

/**
 * Copies a student may have downloaded but never installed. Shown by the
 * Setup check as a suggestion; never run until the Developer picks one.
 */
export function findDownloadedCopies() {
  const found = [];
  const downloads = path.join(os.homedir(), "Downloads");
  const look = (dir) => {
    const p = path.join(dir, EXE);
    if (isExecutableFile(p)) found.push(p);
  };
  look(downloads);
  try {
    for (const d of fs.readdirSync(downloads)) {
      if (/^arduino[-_]cli/i.test(d)) look(path.join(downloads, d));
    }
  } catch {
    /* no Downloads folder */
  }
  return found;
}

let _resolved = null;

/**
 * Where arduino-cli lives, in this order: ARDUINO_CLI_PATH, the copy the
 * Developer picked in Setup, PATH, trusted install folders (a GUI app such
 * as Claude Desktop starts AI Tether with a short PATH that misses Homebrew
 * and WinGet), then the copy Grease installed in ~/.grease/bin. Returns
 * { path, source, onPath } or null.
 */
export function resolveArduinoCli({ refresh = false } = {}) {
  if (_resolved && !refresh && isExecutableFile(_resolved.path)) return _resolved;
  _resolved = null;
  const onPath = findOnPath(EXE);
  const pick = (p, source) => (_resolved = { path: p, source, onPath: !!onPath && path.resolve(onPath) === path.resolve(p) });
  const fromEnv = process.env.ARDUINO_CLI_PATH;
  if (fromEnv && isExecutableFile(fromEnv)) return pick(fromEnv, "env");
  const saved = readGreaseSettings().arduinoCliPath;
  if (typeof saved === "string" && isExecutableFile(saved)) return pick(saved, "saved");
  if (onPath) return pick(onPath, "PATH");
  for (const dir of trustedDirs()) {
    const p = path.join(dir, EXE);
    if (isExecutableFile(p)) return pick(p, "found");
  }
  const own = path.join(GREASE_BIN, EXE);
  if (isExecutableFile(own)) return pick(own, "grease");
  return null;
}

/** One line telling the Developer how to install arduino-cli on this OS. */
export function installLine() {
  if (IS_WIN) return "winget install ArduinoSA.CLI";
  if (IS_MAC) return "brew install arduino-cli";
  return "curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=$HOME/.local/bin sh";
}

/**
 * One line telling the Developer how to put `dir` on PATH on this OS.
 * Windows: append to the USER Path only. Never `setx PATH`, which cuts PATH
 * at 1024 characters and copies the system Path into the user one.
 */
export function pathFixLine(dir) {
  if (IS_WIN) {
    return `powershell -NoProfile -Command "[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path','User') + ';${dir}', 'User')"`;
  }
  if (IS_MAC) return `echo 'export PATH="${dir}:$PATH"' >> ~/.zprofile`;
  return `echo 'export PATH="${dir}:$PATH"' >> ~/.profile`;
}

export function notFoundMessage() {
  return [
    "arduino-cli was not found.",
    "Why: Grease runs arduino-cli for every compile, upload and install, and no copy is on PATH or in the usual install folders.",
    `What: Grease installs it by itself when the editor starts. If that failed: Grease VS sidebar › Help › Install arduino-cli, or run ${installLine()}.`,
  ].join("\n");
}

// ── running it ────────────────────────────────────────────────────────────

/** Kill a child and everything it started (avrdude, esptool, gcc). */
export function killTree(child) {
  if (!child?.pid || child.exitCode !== null) return;
  if (IS_WIN) {
    const taskkill = path.join(process.env.SystemRoot || "C:\\Windows", "System32", "taskkill.exe");
    const k = spawn(taskkill, ["/pid", String(child.pid), "/T", "/F"], { windowsHide: true, shell: false });
    k.on("error", () => {
      try { child.kill(); } catch { /* already gone */ }
    });
    return;
  }
  try {
    process.kill(-child.pid, "SIGKILL"); // detached → its own process group
  } catch {
    try { child.kill("SIGKILL"); } catch { /* already gone */ }
  }
}

/** Commands that never load boards, so they never start the first run. */
const NO_BOARDS = new Set(["version", "config", "help", "completion"]);

/**
 * Run arduino-cli with `args`. Never uses a shell, so paths with spaces and
 * names like "Adafruit NeoPixel" pass through untouched on every OS. A
 * command that loads boards first waits for the first run (ensureFirstRun).
 *
 * @param {string[]} args
 * @param {{ cwd?: string, timeoutMs?: number, env?: Record<string,string>, onData?: (s: string) => void, signal?: AbortSignal, skipFirstRun?: boolean }} [opts]
 * @returns {Promise<{ success: boolean, exitCode: number|null, stdout: string, stderr: string, timedOut?: boolean, notFound?: boolean, cancelled?: boolean, firstRun?: boolean }>}
 */
export async function runCli(args, opts = {}) {
  const cli = resolveArduinoCli();
  if (!cli) {
    return { success: false, exitCode: null, stdout: "", stderr: notFoundMessage(), notFound: true };
  }
  if (!opts.skipFirstRun && !NO_BOARDS.has(args[0])) {
    const fr = await ensureFirstRun();
    if (!fr.success) return fr;
  }
  return spawnCli(cli, args, opts);
}

function spawnCli(cli, args, opts) {
  const timeoutMs = opts.timeoutMs ?? TIMEOUTS.quick;
  // Runs that do not name their own index URLs load no third-party index: a
  // broken or missing URL in the Developer's own config would otherwise print
  // "Error downloading index" on every command, or download at startup.
  const fullArgs = args.includes("--additional-urls") ? args : [...args, "--additional-urls", ""];
  if (opts.signal?.aborted) {
    return Promise.resolve({ success: false, exitCode: null, stdout: "", stderr: "Cancelled.", cancelled: true });
  }
  return new Promise((resolve) => {
    let child;
    try {
      child = spawn(cli.path, fullArgs, {
        cwd: opts.cwd,
        // A GUI app can start AI Tether without TMPDIR, and arduino-cli's ctags then fails with no compiler output.
        env: { ...process.env, ...OFFLINE_ENV, ...(IS_WIN ? {} : { TMPDIR: os.tmpdir() }), ...(opts.env ?? {}) },
        shell: false,
        windowsHide: true,
        detached: !IS_WIN, // own process group, so a timeout can kill the tree
      });
    } catch (e) {
      resolve({ success: false, exitCode: null, stdout: "", stderr: String(e?.message ?? e) });
      return;
    }
    let stdout = "";
    let stderr = "";
    let settled = false;
    const onAbort = () => {
      killTree(child);
      finish({ success: false, exitCode: null, stdout, stderr: "Cancelled.", cancelled: true });
    };
    const finish = (r) => {
      if (settled) return;
      settled = true;
      clearTimeout(timer);
      opts.signal?.removeEventListener("abort", onAbort);
      resolve(r);
    };
    opts.signal?.addEventListener("abort", onAbort, { once: true });
    const timer = setTimeout(() => {
      killTree(child);
      finish({
        success: false,
        exitCode: null,
        stdout,
        stderr: `${stderr}\nStopped after ${Math.round(timeoutMs / 1000)} s (arduino-cli ${args[0] ?? ""} took too long).`,
        timedOut: true,
      });
    }, timeoutMs);
    child.stdout.on("data", (d) => {
      const s = d.toString();
      stdout += s;
      opts.onData?.(s);
    });
    child.stderr.on("data", (d) => {
      const s = d.toString();
      stderr += s;
      opts.onData?.(s);
    });
    child.on("error", (err) => {
      const quarantined = IS_MAC && err?.code === "EACCES";
      finish({
        success: false,
        exitCode: null,
        stdout,
        stderr: quarantined
          ? `${String(err)}\nWhy: macOS blocked ${cli.path} (downloaded file, not executable or quarantined).\nWhat: chmod +x "${cli.path}" && xattr -d com.apple.quarantine "${cli.path}"`
          : `${stderr}\n${String(err)}`,
        notFound: err?.code === "ENOENT",
      });
    });
    child.on("close", (code) => finish({ success: code === 0, exitCode: code, stdout, stderr }));
  });
}

// ── first run ─────────────────────────────────────────────────────────────
//
// arduino-cli downloads its board list, library list and USB tools (~14 MB)
// the first time a command loads boards. Hidden inside a 30 s board scan,
// that fails offline with exit 0 and no ports (the reason is only in
// `warnings`), so Grease runs it on purpose, once, with a long limit.

const FIRST_RUN_LOCK = path.join(GREASE_DIR, "first-run.lock");
const FIRST_RUN_RETRY_MS = 60_000;

let _dataDir = null;

/** arduino-cli's data folder (Arduino15). `config get` works offline and loads no boards. */
export async function cliDataDir() {
  const cli = resolveArduinoCli();
  if (!cli) return null;
  if (_dataDir?.cli === cli.path) return _dataDir.dir;
  const r = await spawnCli(cli, ["config", "get", "directories.data", "--json"], { timeoutMs: TIMEOUTS.quick });
  const dir = r.success ? parseCliJson(r.stdout) : null;
  if (typeof dir !== "string" || !dir) return null; // not cached: retry next call
  _dataDir = { cli: cli.path, dir };
  return dir;
}

/** The first-run parts still missing, by name; [] when arduino-cli is ready (or not found). */
export async function firstRunMissing() {
  const dir = await cliDataDir();
  if (!dir) return [];
  const has = (p, isDir) => {
    try {
      return isDir ? fs.readdirSync(p).length > 0 : fs.statSync(p).size > 0;
    } catch {
      return false;
    }
  };
  return [
    ["board list", path.join(dir, "package_index.json"), false],
    ["library list", path.join(dir, "library_index.json"), false],
    ["USB port finder", path.join(dir, "packages", "builtin", "tools", "serial-discovery"), true],
  ]
    .filter(([, p, isDir]) => !has(p, isDir))
    .map(([name]) => name);
}

const _firstRunListeners = new Set();

/**
 * Listen to the first run: { phase: "start" | "done" | "failed", message? }.
 * Returns a function that stops listening.
 */
export function onFirstRun(fn) {
  _firstRunListeners.add(fn);
  return () => _firstRunListeners.delete(fn);
}

function emitFirstRun(event) {
  for (const fn of _firstRunListeners) {
    try {
      fn(event);
    } catch {
      /* a listener's bug must not stop the run */
    }
  }
}

let _firstRun = null;
let _failure = null;
let _failedAt = 0;

/** The last first-run failure while it stands, or null. */
export function firstRunFailure() {
  return _failure;
}

function firstRunFailed(missing) {
  return {
    success: false,
    exitCode: null,
    stdout: "",
    firstRun: true,
    stderr: [
      `arduino-cli first run failed: ${missing.join(", ")} missing.`,
      "Why: arduino-cli downloads its board list and USB tools once (~14 MB), and the download failed (no internet, or blocked).",
      "What: connect to the internet. Grease retries every minute.",
    ].join("\n"),
  };
}

/**
 * Make sure arduino-cli's first-run download is done. One run at a time in
 * this process (callers share it) and on this computer (a lock file, because
 * every editor window and AI Tether has its own copy of this module). After a
 * failure, calls within a minute get the same failure without a new download,
 * unless `force`.
 *
 * @returns {Promise<{ success: true, ran: boolean } | ReturnType<typeof firstRunFailed>>}
 */
export function ensureFirstRun({ force = false } = {}) {
  if (_firstRun) return _firstRun;
  _firstRun = (async () => {
    const missing = await firstRunMissing();
    if (!missing.length) {
      _failure = null;
      return { success: true, ran: false };
    }
    if (!force && _failure && Date.now() - _failedAt < FIRST_RUN_RETRY_MS) return _failure;
    emitFirstRun({ phase: "start", missing });
    await withFirstRunLock(async () => {
      if (!(await firstRunMissing()).length) return; // another process finished it
      const cli = resolveArduinoCli();
      if (cli) await spawnCli(cli, ["board", "list", "--format", "json"], { timeoutMs: TIMEOUTS.index });
    });
    const still = await firstRunMissing();
    if (!still.length) {
      _failure = null;
      emitFirstRun({ phase: "done" });
      return { success: true, ran: true };
    }
    _failure = firstRunFailed(still);
    _failedAt = Date.now();
    emitFirstRun({ phase: "failed", message: _failure.stderr });
    return _failure;
  })().finally(() => {
    _firstRun = null;
  });
  return _firstRun;
}

function pidAlive(pid) {
  try {
    process.kill(pid, 0);
    return true;
  } catch (e) {
    return e?.code === "EPERM"; // alive, owned by another user
  }
}

/**
 * Run `fn` holding ~/.grease/extension/first-run.lock. Two first runs at once
 * write the same staging files in Arduino15, so a second process waits for
 * the first one's lock (pid inside) to go, then runs `fn` itself, which
 * finds the parts present and returns.
 */
async function withFirstRunLock(fn) {
  const staleMs = TIMEOUTS.index + 30_000;
  const deadline = Date.now() + staleMs;
  try {
    fs.mkdirSync(GREASE_DIR, { recursive: true });
  } catch {
    return fn(); // no lock folder: run unlocked
  }
  for (;;) {
    let fd;
    try {
      fd = fs.openSync(FIRST_RUN_LOCK, "wx");
    } catch (e) {
      if (e?.code !== "EEXIST") return fn();
      let holder = 0;
      let age = 0;
      try {
        holder = Number(fs.readFileSync(FIRST_RUN_LOCK, "utf8")) || 0;
        age = Date.now() - fs.statSync(FIRST_RUN_LOCK).mtimeMs;
      } catch {
        continue; // released between the open and the read
      }
      // An empty lock younger than 5 s is still being written by its owner.
      const held = holder ? holder !== process.pid && pidAlive(holder) && age < staleMs : age < 5000;
      if (held && Date.now() < deadline) {
        await new Promise((r) => setTimeout(r, 1000));
        continue;
      }
      try {
        fs.unlinkSync(FIRST_RUN_LOCK);
      } catch {
        /* someone else removed it */
      }
      continue;
    }
    try {
      fs.writeSync(fd, String(process.pid));
    } finally {
      fs.closeSync(fd);
    }
    try {
      return await fn();
    } finally {
      try {
        if (Number(fs.readFileSync(FIRST_RUN_LOCK, "utf8")) === process.pid) fs.unlinkSync(FIRST_RUN_LOCK);
      } catch {
        /* already gone */
      }
    }
  }
}

// ── WHY + WHAT ────────────────────────────────────────────────────────────

/**
 * Known arduino-cli failures, checked in order against stdout + stderr. Each
 * gives the Developer (and the agent) the reason and the next step, so a
 * missing prerequisite is never mistaken for a broken extension.
 */
const EXPLAIN = [
  {
    test: /arduino-cli was not found/,
    why: "", // notFoundMessage() already says why and what
    what: "",
  },
  {
    test: /--git-url and --zip-path are disabled/i,
    why: "arduino-cli refuses git and zip library installs unless they are allowed for that run.",
    what: "Use Managers › Install library from GitHub; Grease allows it for that one install after you confirm.",
  },
  {
    test: /discovery builtin:\S+ not found|Loading index file: .*package_index\.json/i,
    why: "arduino-cli has not finished its first run: its board list and USB tools are missing (it downloads them once, ~14 MB).",
    what: "Connect to the internet. Grease retries every minute.",
  },
  {
    test: /already installed, but with a different version/i,
    why: "Another version of this library is already installed.",
    what: "Remove the installed copy from your libraries folder, then install again.",
  },
  {
    test: /invalid git url/i,
    why: "That is not a GitHub repository address.",
    what: "Type owner/repo, for example pololu/romi32u4-arduino-library.",
  },
  {
    test: /library not valid|library\.properties|not a valid library|no valid library found/i,
    why: "The repository is not an Arduino library at its top folder (no library.properties or src/).",
    what: "Install the library's own repository, or find it by name in Managers › Library.",
  },
  {
    test: /repository not found|authentication required|could not read Username/i,
    why: "GitHub has no public repository at that address (misspelled, renamed or private).",
    what: "Open the address in a browser, copy owner/repo from it, and try again.",
  },
  {
    test: /reference not found|couldn't find remote ref|invalid reference/i,
    why: "That tag or branch does not exist in the repository.",
    what: "Leave out the #tag to install the default branch, or copy a tag name from the repository's Releases page.",
  },
  {
    test: /library .* not found|No libraries matching/i,
    why: "Library Manager has no library with that exact name.",
    what: "Search in Managers › Library and use the name exactly as listed (with spaces and capitals).",
  },
  {
    test: /platform .* (is )?not (found|installed)|Platform '.*' not found|unknown package|Error during build: Platform/i,
    why: "The board core for this FQBN is not installed.",
    what: "Install it from Managers › Install new board (online), or let the agent call installCore.",
  },
  {
    test: /no such host|dial tcp|connection refused|i\/o timeout|TLS handshake|network is unreachable|Get "https?:/i,
    why: "The download failed: no internet, a proxy, or a firewall blocked it.",
    what: "Check the connection and retry. Grease goes online only for these installs and index updates.",
  },
  {
    test: /permission denied.*\/dev\/tty|\/dev\/tty[^:]*: permission denied/i,
    why: "Linux gives serial ports to the dialout group (uucp on Arch), and this user is not in it.",
    what: "Run `sudo usermod -aG dialout $USER`, then log out and back in.",
  },
  {
    test: /port is busy|Resource busy|Access is denied|could not open port|Serial port busy|cannot open port/i,
    why: "Another program holds the serial port (a Serial Monitor, Arduino IDE, or a second Grease window).",
    what: "Close the other monitor (or Grease › serial off), unplug and replug the board, then retry.",
  },
  {
    test: /No device found on|Couldn't find a Board on the selected port|butterfly_recv|programmer is not responding|No serial data received|Failed to connect to ESP32|Wrong boot mode/i,
    why: "The board did not answer the uploader. On native-USB boards a crashed sketch can hide the USB port.",
    what: "Enter the bootloader by hand (32U4/SAMD/nRF52: double-tap reset; ESP32: hold BOOT, tap RESET; RP2040/RP2350: hold BOOTSEL while plugging in) and upload again.",
  },
  {
    test: /fatal error: (.+?\.h): No such file or directory/i,
    why: "The sketch includes a header that no installed library provides.",
    what: "Install the library that provides it (Managers › Library, or the agent's installLibrary with the exact name).",
  },
];

/**
 * Append a "Why:" and "What:" line to a failed result's stderr, once.
 * @param {{ success: boolean, stdout?: string, stderr?: string, timedOut?: boolean }} result
 */
export function explainCliFailure(result) {
  if (!result || result.success) return result;
  const text = `${result.stderr ?? ""}\n${result.stdout ?? ""}`;
  if (/\nWhy: /.test(text)) return result;
  let why = "";
  let what = "";
  if (result.timedOut) {
    why = "arduino-cli took longer than Grease allows for this step.";
    what = "Retry once (the second build reuses cached files). On a slow link, installs can take several minutes.";
  } else {
    for (const rule of EXPLAIN) {
      const m = text.match(rule.test);
      if (!m) continue;
      why = rule.why;
      what = rule.what;
      if (m[1] && rule.what.startsWith("Install the library")) what = `${m[1]}: ${what}`;
      break;
    }
  }
  if (!why) return result;
  return { ...result, stderr: `${result.stderr ?? ""}\nWhy: ${why}\nWhat: ${what}`.trim() };
}

/** Parse arduino-cli `--json`/`--format json` output, tolerating a banner line before it. */
export function parseCliJson(text) {
  const raw = String(text ?? "");
  try {
    return JSON.parse(raw);
  } catch {
    const m = raw.match(/\{[\s\S]*\}\s*$/) || raw.match(/\[[\s\S]*\]\s*$/);
    if (!m) return null;
    try {
      return JSON.parse(m[0]);
    } catch {
      return null;
    }
  }
}
