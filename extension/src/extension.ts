/**
 * extension.ts
 * ────────────
 * Entry point for Grease VS (formerly Arduino Grease) for VS Code.
 *
 * This file does three big things:
 *
 *   1. **Boot the bundled MCP server.** A Node child process running
 *      `dist/server.mjs` is started on a free local port. The server exposes
 *      a REST API (used by the UI) and an MCP JSON-RPC endpoint (used by AI
 *      assistants like Claude, Cursor, Windsurf, Antigravity).
 *
 *   2. **Reconcile the hardware target.** Runs `arduino-cli board list` when
 *      serial-discovery reports a USB port change (ADR 0014), remembers
 *      boards by USB serial / VID:PID / FQBN so the same physical board
 *      reappears with the right driver, and offers a board-core install
 *      prompt when a new device is plugged in.
 *
 *   3. **Register every `arduinoMcp.*` command.** Each command below is a
 *      named action surfaced in the Command Palette and on the sidebar
 *      toolbar. The block comment above each registerCommand call explains
 *      WHAT the command does — read those if you want to learn how the
 *      extension is wired.
 *
 * New in 2.0.0 (Grease VS):
 *   - The Grease Skill (/grease) is installed into the agents' skill
 *     folders, and the stdio bridge into ~/.grease/bin (ADR 0006, 0007).
 *   - The Help panel replaces the AI Prompt tab: setup checks, AI
 *     Tether, the agents found here, and the skill.
 *   - VS Code gives Copilot the Grease tools through its MCP API.
 *   - An agent's setTarget wins over the automatic board pick.
 *
 * Since v1.0.12: serial I/O via arduino-cli monitor (no native binaries),
 * one universal VSIX for macOS, Linux and Windows, and every new .ino
 * written to disk opens in the editor.
 */

import * as vscode from "vscode";
import * as fs from "node:fs";
import * as os from "node:os";
import * as path from "node:path";

import {
  explainCliFailure,
  extractCoreFromFqbn,
  isCoreInstalled,
  runArduinoCli,
  TIMEOUTS,
  type BoardCandidate,
} from "./arduinoCli";
import { ensureFirstRun, onFirstRun, pathFixLine, resolveArduinoCli } from "../server/lib/cli.mjs";
import { CLI_VERSION, cliAsset, installArduinoCli } from "../server/lib/cliInstall.mjs";
import { installSupportedCore } from "../server/lib/install.mjs";
import { SUPPORTED_CORES, supportedCore } from "../server/lib/supported.mjs";
import { canProbe, mcuOfFqbn } from "../server/lib/espProbe.mjs";
import { addTools, detectAgents, installSkill, removeSkill, skillStatus } from "../server/lib/agents.mjs";
import { SKILL_TEXT } from "../server/lib/skill.mjs";
import { installGrease } from "./greaseSetup";
import { PORT_GONE_MS, PortGoneTimer } from "./portGone";
import { startSerialDiscovery } from "./serialDiscovery";
import {
  boardMemoryKeys,
  loadBoardMemory,
  promptForTarget,
  refreshBoards,
  saveBoardMemory,
  saveTarget,
  vidPidKey,
  type Target,
} from "./boards";
import { parseArduinoDocs, registerArduinoHoverProvider } from "./arduinoHover";
import {
  findMainSketchFile,
  getActiveInoPath,
  getSketchFolder,
  prepareSketchForBuild,
  type PreparedSketch,
} from "./sketch";
import { chipOf, editorHeaders, postCompile, postUpload, setAuthKey, setServerBaseUrl, signalIdle, signalThrust, syncTargetToServer } from "./serverHttpClient";
import { connectTether, type ServerHandle } from "./serverProcess";
import { ExamplesPanel } from "./ui/examplesPanel";
import { SerialMonitorPanel } from "./ui/serialMonitorPanel";
import { SerialPlotterPanel } from "./ui/serialPlotterPanel";
import { ArduinoToolbarViewProvider, bubbleOf } from "./views/toolbarView";

const OUTPUT_CHANNEL_NAME = "Grease VS";
/** Set when the Developer removed the skill: activation must not put it back. */
const SKILL_REMOVED_KEY = "grease.skillRemoved";
/** The one-time "add the Grease tools to your agents?" question was asked. */
const TOOLS_OFFERED_KEY = "grease.toolsOffered";

function delay(ms: number): Promise<void> {
  return new Promise((resolve) => setTimeout(resolve, ms));
}


export async function activate(context: vscode.ExtensionContext): Promise<void> {
  const output = vscode.window.createOutputChannel(OUTPUT_CHANNEL_NAME);
  context.subscriptions.push(output);

  // Ensure the canonical sketchbook exists. All agent-created sketches live here.
  const SKETCHBOOK_DIR = path.join(os.homedir(), "Grease");
  try { fs.mkdirSync(SKETCHBOOK_DIR, { recursive: true }); } catch (_e) {}

  // Absolute path to the bundled hover-doc sidecar header.
  const sidecarPath = context.asAbsolutePath(path.join("resources", "arduino_docs.h"));

  // Parse arduino_docs.h once and register Grease's own hover provider.
  // This covers all documented Arduino core symbols with brief + signature + @params.
  const docsMap = parseArduinoDocs(sidecarPath);
  context.subscriptions.push(registerArduinoHoverProvider(docsMap, output));

  // Watch for new .ino files created on disk by any means (AI agent, script,
  // file copy) — open them in the editor immediately so the sketch is always
  // visible to the user the moment it appears.
  const inoWatcher = vscode.workspace.createFileSystemWatcher("**/*.ino");
  inoWatcher.onDidCreate((uri) => {
    output.appendLine(`[Grease] New sketch detected: ${uri.fsPath}`);
    vscode.workspace.openTextDocument(uri).then(
      (doc) => vscode.window.showTextDocument(doc, { preview: false }),
      () => { /* file may have moved before we opened it */ },
    );
  });
  context.subscriptions.push(inoWatcher);

  // ── First-install setup: apply the Grease theme + move Activity Bar to top.
  //    The key is versioned so future major updates can re-apply themes if needed.
  vscode.workspace
    .getConfiguration()
    .update("output.smartScroll.enabled", false, vscode.ConfigurationTarget.Global)
    .then(undefined, () => {});
  const FIRST_INSTALL_KEY = "arduinoMcp.firstInstallDone_1_0_6";
  const firstInstallDone = context.globalState.get(FIRST_INSTALL_KEY);
  if (!firstInstallDone) {
    try {
      const wbConfig = vscode.workspace.getConfiguration("workbench");
      await wbConfig.update("colorTheme", "Grease", vscode.ConfigurationTarget.Global);
      await wbConfig.update("activityBar.location", "top", vscode.ConfigurationTarget.Global);
      output.appendLine("First install: Applied Grease theme and moved Activity Bar to top.");
    } catch (e) {
      output.appendLine(
        "First install theme setup failed: " + (e instanceof Error ? e.message : String(e)),
      );
    }
    await context.globalState.update(FIRST_INSTALL_KEY, true);
  }

  // ── Diagnostics collection for Compile errors.
  //    Populated by parsing arduino-cli stderr; clangd has its own collection
  //    (provided by the clangd extension) for in-editor squiggles.
  const arduinoDiagnostics = vscode.languages.createDiagnosticCollection("arduino");
  context.subscriptions.push(arduinoDiagnostics);

  // ── Bundled MCP server lifecycle ────────────────────────────────────────
  let serverProcess: ServerHandle | null = null;
  let serverHealthy = false;
  let sawServerProblem = false;
  let lastScanAtMs: number | null = null;
  // arduino-cli missing: the sidebar shows "Setup needed" until a check finds it.
  let setupNeeded = false;
  // Settles when arduino-cli is installed and its first run is done (or failed):
  // board scans wait for it instead of logging "not found" during the install.
  let cliReady: Promise<unknown> = Promise.resolve();
  // Last agent setTarget the extension adopted (AI Tether's targetVersion).
  let adoptedAgentTarget = -1;

  // VS Code hands the Grease tools to Copilot through its own MCP API: no
  // config file, no bridge, no Node.js. Other editors use the bridge.
  const mcpDefinitionsChanged = new vscode.EventEmitter<void>();
  context.subscriptions.push(mcpDefinitionsChanged);
  if (
    /^visual studio code/i.test(vscode.env.appName) &&
    typeof vscode.lm?.registerMcpServerDefinitionProvider === "function"
  ) {
    context.subscriptions.push(
      vscode.lm.registerMcpServerDefinitionProvider("grease.aiTether", {
        onDidChangeMcpServerDefinitions: mcpDefinitionsChanged.event,
        provideMcpServerDefinitions: () =>
          serverProcess
            ? [
                new vscode.McpHttpServerDefinition(
                  "Grease",
                  vscode.Uri.parse(`http://127.0.0.1:${serverProcess.port}/mcp`),
                  { "x-grease-auth": serverProcess.authKey },
                  String(context.extension.packageJSON.version),
                ),
              ]
            : [],
      }),
    );
  }

  const startServer = async (): Promise<boolean> => {
    if (serverProcess) return true;
    try {
      serverProcess = await connectTether(context, output, 3333, vscode.env.appName);
      setServerBaseUrl(`http://127.0.0.1:${serverProcess.port}`);
      setAuthKey(serverProcess.authKey);
      mcpDefinitionsChanged.fire();
      return true;
    } catch (e) {
      output.appendLine(
        `Failed to start AI Tether: ${e instanceof Error ? e.message : String(e)}`,
      );
      return false;
    }
  };

  const stopServer = async (): Promise<void> => {
    if (!serverProcess) return;
    await serverProcess.stop();
    serverProcess = null;
    serverHealthy = false;
    sawServerProblem = false;
    mcpDefinitionsChanged.fire();
    output.appendLine("AI Tether stopped on this computer.");
  };

  // Closing the window leaves AI Tether running: other windows and agents may
  // use it, and it exits by itself after ~10 minutes with no client (ADR 0008).
  await startServer();

  // ── Status bar: shows the current target (board @ port) and is clickable
  //    to re-pick. Warning state when no target is set.
  const status = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
  status.name = "Grease Target";
  status.command = "arduinoMcp.refreshPortsBoards";
  status.show();
  context.subscriptions.push(status);

  // ── UI panel instances ──────────────────────────────────────────────────
  const serialMonitorPanel = new SerialMonitorPanel(output, (line) => toolbar.postSerialLine(line));
  const serialPlotterPanel = new SerialPlotterPanel(output, context.extensionUri);
  const examplesPanel = new ExamplesPanel(context, output);

  // Forward declarations for closures the toolbar needs to call.
  let lastCandidates: BoardCandidate[] = [];
  let currentTarget: Target | null = null;

  // ── Sidebar toolbar (the dark "command center" the user lives in) ───────
  const toolbar = new ArduinoToolbarViewProvider(context, output, {
    /**
     * User picked a board in the Managers tab. Find a matching port (prefer
     * one whose detected board family matches the chosen FQBN), save it,
     * push it to the server, and remember the (USB id → FQBN) mapping so the
     * same board reappears with the right driver next time.
     */
    chooseTarget: async (fqbn: string) => {
      const corePrefix = fqbn.split(":").slice(0, 2).join(":");
      const candidate =
        lastCandidates.find(
          (c) => c.fqbn?.startsWith(corePrefix + ":") && typeof c.port === "string",
        ) ??
        lastCandidates.find((c) => c.port === currentTarget?.port) ??
        lastCandidates.find((c) => typeof c.port === "string") ??
        null;
      const port = candidate?.port ?? currentTarget?.port ?? null;
      const memCandidate = candidate ?? (port ? lastCandidates.find((c) => c.port === port) : null);
      const keysToSave = boardMemoryKeys(memCandidate ?? null);
      if (keysToSave.length > 0) {
        const mem = await loadBoardMemory(context);
        for (const k of keysToSave) mem[k] = { fqbn };
        await saveBoardMemory(context, mem);
        output.appendLine(`[BoardMemory] Saved (${keysToSave.join(", ")}) → ${fqbn}`);
      } else if (port) {
        output.appendLine(
          `[BoardMemory] Warning: no identifiers found for ${port} — memory not saved`,
        );
      }
      currentTarget = { port, fqbn, userChosen: true };
      await saveTarget(context, currentTarget);
      await syncTargetToServer(currentTarget);
      if (port) {
        setOk(currentTarget);
      } else {
        setWarning(`${fqbn} — no port`);
        vscode.window.showInformationMessage(
          `Grease: Board type set to ${fqbn}. Connect the board and use Recovery Upload.`,
        );
      }
      refreshToolbarState();
    },
    /**
     * User chose "Upload firmware to target" — this is the bootloader-burn
     * flow, used when initially programming a bare chip. Sets the target
     * and then runs `arduino-cli burn-bootloader`.
     */
    recoveryUpload: async () => {
      await vscode.commands.executeCommand("arduinoMcp.recoveryUpload");
    },
    uploadFirmwareToTarget: async (fqbn: string) => {
      const port = currentTarget?.port ?? lastCandidates[0]?.port ?? null;
      if (!port) {
        vscode.window.showWarningMessage(
          "Grease: No port detected. Connect a board first.",
        );
        return;
      }
      currentTarget = { port, fqbn, userChosen: true };
      await saveTarget(context, currentTarget);
      await syncTargetToServer(currentTarget);
      setOk(currentTarget);
      refreshToolbarState();
      await runFirmwareUpload(fqbn, port);
    },
    /** Close the serial port when the user navigates away from the Board tab. */
    serialOff: async () => {
      if (serialMonitorPanel.isConnected) {
        await serialMonitorPanel.stop();
        output.appendLine("Serial disconnected (panel switch).");
        refreshToolbarState();
      }
    },
    /** Apply a new accent color across status bar, focus border, and badges. */
    cycleAccent: async (color: string) => {
      const config = vscode.workspace.getConfiguration("workbench");
      const current = (config.get("colorCustomizations") as Record<string, string>) || {};
      const updated = {
        ...current,
        "statusBar.background": color,
        "statusBar.noFolderBackground": color,
        "statusBar.debuggingBackground": color,
        "statusBarItem.remoteBackground": color,
        focusBorder: color,
        "activityBarBadge.background": color,
        "panelTitle.activeBorder": color,
      };
      const target =
        vscode.workspace.workspaceFolders && vscode.workspace.workspaceFolders.length > 0
          ? vscode.ConfigurationTarget.Workspace
          : vscode.ConfigurationTarget.Global;
      await config.update("colorCustomizations", updated, target);
      output.appendLine(
        `Accent color set to ${color} (Target: ${target === vscode.ConfigurationTarget.Workspace ? "Workspace" : "Global"})`,
      );
      refreshToolbarState();
    },
    /**
     * NEW in v1.0.9: called by the toolbar after a successful
     * `lib install` (registry name OR git URL). Regenerates clangd's
     * compile database so freshly added headers become hover-able.
     */
    onBoardInstalled: async () => { await reconcileTarget(); },
    onSetupChecked: (cliFound: boolean) => {
      setupNeeded = !cliFound;
      refreshToolbarState();
    },
    prepareCli: async () => {
      const ok = await prepareArduinoCli(true);
      if (ok) await reconcileTarget();
      return ok;
    },
  });
  context.subscriptions.push(
    vscode.window.registerWebviewViewProvider(ArduinoToolbarViewProvider.viewType, toolbar),
  );

  /**
   * Add the Grease tools to the agents found here. Each agent's config gets
   * one "grease" entry that runs the stdio bridge, and Grease keeps a backup
   * of every file it changes. Always asks first: these are the Developer's
   * config files.
   */
  const offerAgentTools = async (fromCommand: boolean): Promise<void> => {
    const missing = detectAgents({ hostApp: vscode.env.appName }).filter(
      (a) => a.present && (a.tools === "missing" || a.tools === "old"),
    );
    if (!missing.length) {
      if (fromCommand) void vscode.window.showInformationMessage("Grease: every agent found here already has the Grease tools.");
      return;
    }
    if (!fromCommand) {
      await context.globalState.update(TOOLS_OFFERED_KEY, true);
      const answer = await vscode.window.showInformationMessage(
        `Grease: add Grease tools to ${missing.map((a) => a.name).join(", ")}?`,
        "Choose",
        "Not now",
      );
      if (answer !== "Choose") return;
    }
    const picks = await vscode.window.showQuickPick(
      missing.map((a) => ({
        label: a.name,
        description: a.tools === "old" ? "replaces the old Arduino Grease entry" : "",
        detail: a.configFile ?? undefined,
        picked: true,
        id: a.id,
      })),
      { canPickMany: true, title: "Add the Grease tools to these agents (a backup of each config is kept)" },
    );
    if (!picks?.length) return;
    const added: string[] = [];
    for (const p of picks) {
      const r = await addTools(p.id, { editorNode: process.execPath });
      output.appendLine(`[Grease] Add tools (${p.label}): ${r.message}${r.manual ? ` ${r.manual}` : ""}`);
      if (r.ok) added.push(p.label);
      if (!r.ok && r.manual) {
        const copy = await vscode.window.showWarningMessage(`Grease: ${p.label}: ${r.message} ${r.manual}`, "Copy command");
        if (copy) await vscode.env.clipboard.writeText(r.manual);
      } else if (!r.ok) {
        void vscode.window.showWarningMessage(`Grease: ${p.label}: ${r.message}`);
      }
    }
    if (added.length) void vscode.window.showInformationMessage(`Grease: tools added to ${added.join(", ")}. Restart them.`);
    toolbar.postGrease();
  };

  // ── Grease Skill + stdio bridge (ADR 0006, 0007) ─────────────────────────
  //    The bridge is Grease's own file. The skill goes into the agents' skill
  //    folders on every activation (updates replace Grease's unedited copies),
  //    unless the Developer removed it.
  {
    const skillWanted = !context.globalState.get<boolean>(SKILL_REMOVED_KEY, false);
    const { skills } = installGrease(context, output, { skill: skillWanted });
    const fresh = skills.filter((s) => s.action === "installed" || s.action === "replaced");
    if (fresh.length) {
      for (const f of fresh) output.appendLine(`[Grease] Skill for ${f.readers}: ${f.file}`);
      void vscode.window
        .showInformationMessage("Grease: /grease skill installed for your agents.", "Show", "Remove")
        .then((action) => {
          if (action === "Show") void vscode.commands.executeCommand("arduinoMcp.showSkill");
          else if (action === "Remove") void vscode.commands.executeCommand("arduinoMcp.removeSkill");
        });
    }
    if (!context.globalState.get<boolean>(TOOLS_OFFERED_KEY, false)) {
      // After the skill notice, so the two questions do not land at once.
      const timer = setTimeout(() => void offerAgentTools(false), fresh.length ? 4000 : 1500);
      context.subscriptions.push({ dispose: () => clearTimeout(timer) });
    }
  }

  // ── Detection loop state ────────────────────────────────────────────────
  let lastPorts: Set<string> = new Set();
  let lastServerState: any = null;
  let lastBusyAtMs = 0;
  const portGone = new PortGoneTimer();
  // One follow-up scan when the target's port is missing but still inside
  // PORT_GONE_MS: board scans run only on port events (ADR 0014), so without
  // it an unplugged target would stay until the next event.
  let portGoneRecheck: ReturnType<typeof setTimeout> | null = null;
  context.subscriptions.push({ dispose: () => { if (portGoneRecheck) clearTimeout(portGoneRecheck); } });
  let lastPortChangeVersion = 0;
  let isPickerOpen = false;
  // Intentionally do NOT restore the persisted target at activation.
  //
  // VS Code globalState survives a reinstall, so loading the last-used
  // target made the UI "guess" a board (e.g. a previously-used
  // Seeed XIAO ESP32C6) even when nothing was plugged in — confusing
  // and contrary to the 1.0.8 behavior where startup was always blank.
  // boardMemory (USB serial → FQBN) still handles fast reconnect inside
  // `reconcileTarget`, so an actually-connected board with a known USB
  // identity is restored as soon as `arduino-cli board list` runs.
  // AI Tether's own target is left alone: other windows and agents share it
  // (ADR 0008), and the health poll adopts a Developer's or agent's choice.
  currentTarget = null;

  const refreshToolbarState = (): void => {
    const config = vscode.workspace.getConfiguration("workbench");
    const customizations = (config.get("colorCustomizations") as Record<string, string>) || {};
    toolbar.setState({
      port: currentTarget?.port ?? null,
      fqbn: currentTarget?.fqbn ?? null,
      connectedPorts: Array.from(lastPorts),
      serverRunning: serverProcess !== null,
      serverHealthy,
      lastScanAtMs,
      serialActive: serialMonitorPanel.isConnected,
      uploading: !!lastServerState?.uploading,
      accentColor: customizations["statusBar.background"] || "#007ACC",
      setupNeeded,
      tetherPort: serverProcess?.port ?? null,
    });
  };

  const setWarning = (text: string): void => {
    status.text = `$(warning) ${text}`;
    status.backgroundColor = new vscode.ThemeColor("statusBarItem.warningBackground");
    status.tooltip = "Grease needs a board/port selection";
  };

  const setOk = (target: Target): void => {
    if (target.fqbn && target.port) {
      status.text = `$(circuit-board) ${target.fqbn} @ ${target.port}`;
      status.backgroundColor = undefined;
      status.tooltip = "Grease target";
    } else if (target.fqbn && !target.port) {
      status.text = `$(circuit-board) ${target.fqbn} — Recovery mode`;
      status.backgroundColor = new vscode.ThemeColor("statusBarItem.warningBackground");
      status.tooltip = "Board type set, no port detected. Use Recovery Upload.";
    } else {
      status.text = `$(plug) ${target.port} (board unknown)`;
      status.backgroundColor = new vscode.ThemeColor("statusBarItem.warningBackground");
      status.tooltip =
        "Port detected but board not resolved. Use 'arduino-cli board list' and 'arduino-cli core install <package>' to install the correct driver.";
    }
  };

  // ── Necessary installs (ADR 0012) ───────────────────────────────────────
  //    arduino-cli, its first run and a recognized board's core install
  //    without asking. Progress shows in the status bar, in place of the
  //    target item; the result is one short pop-up.
  const job = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
  job.name = "Grease Install";
  context.subscriptions.push(job);
  const showJob = (text: string, opts: { tooltip?: string; command?: string; error?: boolean } = {}): void => {
    job.text = text;
    job.tooltip = opts.tooltip;
    job.command = opts.command ?? "arduinoMcp.openGreasePanel";
    job.backgroundColor = opts.error ? new vscode.ThemeColor("statusBarItem.errorBackground") : undefined;
    job.show();
    status.hide();
  };
  const endJob = (): void => {
    job.hide();
    status.show();
  };

  const FIRST_RUN_MB = 14;
  let installedCliThisSession = false;
  let firstRunFailShown = false;
  let firstRunFailedBefore = false;

  const retrySetup = (): void => {
    void prepareArduinoCli(true).then((ok) => ok && reconcileTarget());
  };

  onFirstRun((e) => {
    if (e.phase === "start") {
      showJob(`$(sync~spin) Grease: first run · ${FIRST_RUN_MB} MB`, {
        tooltip: "arduino-cli downloads its board list and USB tools once",
      });
      toolbar.setJob({ kind: "firstRun", label: `first run · ${FIRST_RUN_MB} MB` });
      output.appendLine("[Setup] arduino-cli first run: downloading its board list and USB tools (once)...");
    } else if (e.phase === "done") {
      endJob();
      toolbar.setJob(null);
      output.appendLine("[Setup] arduino-cli first run done.");
      const msg = installedCliThisSession
        ? `Grease: arduino-cli ${CLI_VERSION} installed and ready (${Math.round((cliAsset()?.bytes ?? 17e6) / 1e6) + FIRST_RUN_MB} MB, once).`
        : `Grease: arduino-cli ready (${FIRST_RUN_MB} MB, once).`;
      installedCliThisSession = false;
      firstRunFailShown = false;
      firstRunFailedBefore = false;
      void vscode.window.showInformationMessage(msg);
      toolbar.refreshCli();
    } else {
      showJob("$(warning) Grease: first run failed", {
        tooltip: "No internet, or blocked. Grease retries every minute. Click for Help.",
        error: true,
      });
      toolbar.setJob({ kind: "firstRun", label: "first run", failed: "no internet, or blocked" });
      if (!firstRunFailedBefore) output.appendLine(`[Setup] ${e.message ?? "arduino-cli first run failed."}`);
      firstRunFailedBefore = true;
      if (firstRunFailShown) return; // background retries stay quiet
      firstRunFailShown = true;
      void vscode.window
        .showErrorMessage("Grease: arduino-cli first run failed (no internet, or blocked).", "Retry")
        .then((a) => a === "Retry" && retrySetup());
    }
  });

  /** Install arduino-cli when none is found, then make sure its first run is done. */
  let cliPrep: Promise<boolean> | null = null;
  const prepareArduinoCli = (force = false): Promise<boolean> => {
    if (cliPrep) return cliPrep;
    cliPrep = (async () => {
      const found = resolveArduinoCli({ refresh: true });
      if (!found) {
        const mb = Math.round((cliAsset()?.bytes ?? 17e6) / 1e6);
        const label = `arduino-cli ${CLI_VERSION}`;
        showJob(`$(sync~spin) Grease: ${label} · 0% of ${mb} MB`, { tooltip: "Grease installs arduino-cli into ~/.grease/bin" });
        toolbar.setJob({ kind: "cli", label, fraction: 0 });
        output.appendLine(`[Setup] arduino-cli not found: installing ${label} (${mb} MB) into ~/.grease/bin...`);
        let lastPct = -1;
        const r = await installArduinoCli({
          onProgress: (f) => {
            const pct = Math.floor(f * 100);
            if (pct === lastPct) return;
            lastPct = pct;
            job.text = `$(sync~spin) Grease: ${label} · ${pct}% of ${mb} MB`;
            if (pct % 5 === 0) toolbar.setJob({ kind: "cli", label, fraction: f });
          },
        });
        if (!r.success) {
          const error = "error" in r ? r.error : "unknown error";
          setupNeeded = true;
          refreshToolbarState();
          output.appendLine(`[Setup] arduino-cli did not install: ${error}`);
          showJob("$(warning) Grease: arduino-cli not installed", { tooltip: error, error: true });
          toolbar.setJob({ kind: "cli", label, failed: error });
          void vscode.window
            .showErrorMessage(`Grease: arduino-cli did not install (${error}).`, "Retry", "Install by hand")
            .then((a) => {
              if (a === "Retry") retrySetup();
              else if (a === "Install by hand") void vscode.commands.executeCommand("arduinoMcp.openGreasePanel");
            });
          return false;
        }
        installedCliThisSession = true;
        setupNeeded = false;
        endJob();
        toolbar.setJob(null);
        refreshToolbarState();
        output.appendLine(`[Setup] Installed arduino-cli ${r.version}: ${r.path}`);
      } else if (found.source === "grease") {
        output.appendLine(`[Setup] arduino-cli: ${found.path} (installed by Grease)`);
      } else if (!found.onPath) {
        output.appendLine(`[Setup] arduino-cli: ${found.path} (found in a ${found.source === "saved" ? "folder you picked" : "standard install folder"}, NOT on PATH).`);
        output.appendLine(`[Setup] Grease uses it directly; terminals and other agents will not find it until you run: ${pathFixLine(path.dirname(found.path))}`);
      } else {
        output.appendLine(`[Setup] arduino-cli: ${found.path}`);
      }
      const fr = await ensureFirstRun({ force });
      if (fr.success && !fr.ran && installedCliThisSession) {
        // Arduino15 was already complete (Arduino IDE 2 shares it): no first run.
        installedCliThisSession = false;
        void vscode.window.showInformationMessage(
          `Grease: arduino-cli ${CLI_VERSION} installed (${Math.round((cliAsset()?.bytes ?? 17e6) / 1e6)} MB).`,
        );
      }
      if (fr.success) {
        endJob();
        toolbar.setJob(null);
      }
      toolbar.refreshCli();
      return fr.success;
    })().finally(() => {
      cliPrep = null;
    });
    return cliPrep;
  };

  /** Hit `/state` and update the toolbar's health indicator. */
  const checkServerHealth = async (verbose = false): Promise<void> => {
    if (!serverProcess) {
      serverHealthy = false;
      toolbar.setBubble("idle");
      refreshToolbarState();
      return;
    }
    try {
      const res = await fetch(`http://127.0.0.1:${serverProcess.port}/state`, {
        headers: editorHeaders(serverProcess.authKey),
      });
      const s: any = await res.json();
      const ok = !!s.target;
      serverHealthy = ok;

      toolbar.setBubble(bubbleOf(s));
      lastServerState = s;
      if (s.uploading || s.compiling) lastBusyAtMs = Date.now();
      if (
        typeof s.portChangeVersion === "number" &&
        s.portChangeVersion !== lastPortChangeVersion
      ) {
        lastPortChangeVersion = s.portChangeVersion;
        if (!s.uploading) void reconcileTarget();
      }

      // An agent (setTarget) or the Developer in another window chose the
      // board: adopt it as the Developer's choice, so the automatic pick in
      // reconcileTarget leaves it alone.
      if (
        (s.targetBy === "agent" || s.targetBy === "developer") &&
        typeof s.targetVersion === "number" &&
        s.targetVersion !== adoptedAgentTarget
      ) {
        adoptedAgentTarget = s.targetVersion;
        if (s.target?.fqbn) {
          currentTarget = { port: s.target.port ?? null, fqbn: s.target.fqbn, userChosen: true };
          await saveTarget(context, currentTarget);
          setOk(currentTarget);
          output.appendLine(`[Grease] ${s.targetBy === "agent" ? "Agent" : "Developer"} chose ${s.target.fqbn} @ ${s.target.port ?? "no port"}.`);
        }
      }

      // Log a problem once, not every 500 ms while it lasts.
      if (!ok) {
        if (!sawServerProblem) output.appendLine("Problem with AI Tether.");
        sawServerProblem = true;
      } else if (sawServerProblem) {
        output.appendLine("AI Tether recovered.");
        sawServerProblem = false;
      }
      if (verbose) {
        output.appendLine(`Server status: ${ok ? "running and healthy" : "running but unhealthy"}`);
      }
    } catch (e) {
      serverHealthy = false;
      toolbar.setBubble("idle");
      if (!sawServerProblem) output.appendLine("Problem with AI Tether: it does not answer.");
      if (verbose) {
        output.appendLine(
          `Server health check failed: ${e instanceof Error ? e.message : String(e)}`,
        );
      }
      sawServerProblem = true;
    }
    refreshToolbarState();
  };

  // Tracks which board+port combos have already been logged by BoardMemory
  // so the 5-second reconcile loop doesn't spam the Output channel on every cycle.
  const loggedRestores = new Set<string>();

  // Ports already asked about since they were plugged in ("port|core").
  const askedForCore = new Set<string>();

  /** Install one Supported Core, with progress in the status bar (click to cancel), thrust and WHY + WHAT on failure. */
  let coreAbort: AbortController | null = null;
  const installCoreWithProgress = async (coreId: string): Promise<boolean> => {
    const core = supportedCore(coreId);
    if (!core || coreAbort) return false;
    const short = core.name.split(" (")[0];
    output.appendLine(`[Grease] Installing ${core.name} (${core.id}, online)...`);
    try { await signalThrust(); } catch { /* AI Tether may be off */ }
    coreAbort = new AbortController();
    const cancel = "arduinoMcp.cancelInstall";
    showJob(`$(sync~spin) Grease: ${short} · starting (click to cancel)`, { command: cancel, tooltip: `Installing ${core.id}` });
    toolbar.setJob({ kind: "core", label: short });
    let res;
    try {
      res = explainCliFailure(
        await installSupportedCore(core.id, {
          signal: coreAbort.signal,
          onData: (d) => {
            output.append(d);
            const line = d.split(/[\r\n]+/).map((l) => l.trim()).filter(Boolean).pop();
            if (line) job.text = `$(sync~spin) Grease: ${short} · ${line.slice(0, 40)} (click to cancel)`;
          },
        }),
      );
    } finally {
      coreAbort = null;
      endJob();
      toolbar.setJob(null);
      try { await signalIdle(); } catch { /* ignore */ }
    }
    if (res.cancelled) {
      output.appendLine(`[Grease] ${core.name}: install cancelled.`);
      return false;
    }
    if (!res.success) {
      output.appendLine(res.stderr);
      const why = res.stderr.split("\n").find((l) => l.startsWith("Why:"))?.slice(5) ?? "See Output › Grease VS.";
      void vscode.window
        .showErrorMessage(`Grease: ${short} did not install. ${why}`, "Retry")
        .then((a) => {
          if (a === "Retry") void installCoreWithProgress(core.id).then((ok) => ok && reconcileTarget());
        });
      return false;
    }
    void vscode.window.showInformationMessage(`Grease: ${short} installed.`);
    return true;
  };
  context.subscriptions.push(
    vscode.commands.registerCommand("arduinoMcp.cancelInstall", () => coreAbort?.abort()),
  );

  /** Install the core a just-plugged-in board needs, once per plug-in, without asking (ADR 0012). */
  const installMissingCore = async (coreId: string, port: string | null): Promise<void> => {
    const core = supportedCore(coreId);
    const key = `${port}|${coreId}`;
    if (!core || askedForCore.has(key)) return;
    askedForCore.add(key);
    if (await isCoreInstalled(coreId)) return;
    if (await installCoreWithProgress(core.id)) await reconcileTarget();
  };

  /**
   * Walk `arduino-cli board list`, apply remembered FQBNs to ports whose
   * USB identity we've seen before, then decide whether to auto-set a
   * target (only when there's exactly one unambiguous candidate and the
   * user hasn't chosen one explicitly).
   */
  const reconcileTargetOnce = async (): Promise<void> => {
    await cliReady;
    lastScanAtMs = Date.now();
    const detection = await refreshBoards(output);
    lastCandidates = detection.candidates;
    // One USB ID for many ESP32 chips (303a:1001 is every C3/C6/S3): AI Tether
    // reads the chip once per board, and that chip's FQBN replaces the guesses.
    const chips = new Map<string, { chip: string; mcu: string | null }>();
    for (const port of new Set(lastCandidates.map((c) => c.port))) {
      const onPort = lastCandidates.filter((c) => c.port === port);
      const c = onPort[0];
      const usbId = c.vid && c.pid ? `${String(c.vid).replace(/^0x/i, "")}:${String(c.pid).replace(/^0x/i, "")}`.toLowerCase() : "";
      if (!canProbe(usbId) || (onPort.length === 1 && c.fqbn)) continue;
      const probe = await chipOf({ port, usbId, serialNumber: c.serialNumber ?? null });
      if (!probe?.chip) continue;
      chips.set(port, { chip: probe.chip, mcu: probe.mcu });
      const named = { ...c, name: `${probe.chip} (chip read over USB)`, fqbn: probe.fqbn ?? undefined };
      lastCandidates = lastCandidates.filter((x) => x.port !== port).concat(named);
    }
    const portsNow = new Set(
      lastCandidates.map((c) => c.port).filter((p): p is string => typeof p === "string"),
    );
    const changed =
      portsNow.size !== lastPorts.size ||
      Array.from(portsNow).some((p) => !lastPorts.has(p)) ||
      Array.from(lastPorts).some((p) => !portsNow.has(p));
    lastPorts = portsNow;
    if (changed) loggedRestores.clear();
    // Forget "already asked" for unplugged ports, so plugging in again asks again.
    for (const key of Array.from(askedForCore)) {
      if (!portsNow.has(key.split("|")[0])) askedForCore.delete(key);
    }
    if (portGoneRecheck) {
      clearTimeout(portGoneRecheck);
      portGoneRecheck = null;
    }
    if (currentTarget?.port && portGone.isGone(portsNow.has(currentTarget.port), lastBusyAtMs)) {
      output.appendLine(`Port ${currentTarget.port} disconnected. Clearing selected target.`);
      portGone.reset();
      currentTarget = null;
      await saveTarget(context, null);
      await syncTargetToServer(null);
    } else if (currentTarget?.port && !portsNow.has(currentTarget.port)) {
      portGoneRecheck = setTimeout(() => {
        portGoneRecheck = null;
        // During an upload the post-upload rescan (portChangeVersion) takes over.
        if (!lastServerState?.uploading) void reconcileTarget();
      }, PORT_GONE_MS + 250);
    }
    // Apply board memory, then dedupe to one best candidate per port.
    const boardMem = await loadBoardMemory(context);
    const resolvedCandidates: (BoardCandidate & { fromMemory?: boolean })[] = [];
    for (const c of lastCandidates) {
      resolvedCandidates.push(await (async () => {
        const chip = chips.get(c.port);
        for (const k of boardMemoryKeys(c)) {
          if (boardMem[k]) {
            // A remembered FQBN for another chip (an S3's, on a port now holding a C6) does not apply.
            if (chip?.mcu) {
              const mcu = await mcuOfFqbn(boardMem[k].fqbn);
              if (mcu && mcu !== chip.mcu) continue;
            }
            const logKey = `${c.port}:${k}:${boardMem[k].fqbn}`;
            if (!loggedRestores.has(logKey)) {
              output.appendLine(`[BoardMemory] Restored ${boardMem[k].fqbn} for ${c.port} via ${k}`);
              loggedRestores.add(logKey);
            }
            return { ...c, fqbn: boardMem[k].fqbn, fromMemory: true };
          }
        }
        return c;
      })());
    }
    const portMap = new Map<string, BoardCandidate & { fromMemory?: boolean }>();
    for (const c of resolvedCandidates) {
      if (typeof c.port !== "string") continue;
      const existing = portMap.get(c.port);
      if (!existing || (!existing.fromMemory && c.fromMemory)) portMap.set(c.port, c);
    }
    const deduped = Array.from(portMap.values());
    const candidatesWithFqbn = deduped.filter(
      (c) => typeof c.fqbn === "string" && typeof c.port === "string",
    );
    // Only USB ports can be boards: macOS always lists /dev/cu.debug-console and
    // Bluetooth ports, Windows desktops a legacy COM1, all without a USB VID.
    const portsOnly = deduped.filter((c) => !c.fqbn && typeof c.port === "string" && !!c.vid);
    // The target must build for the chip on its port, whoever chose it: the
    // board on that port is not the one the target was chosen for.
    const chipHere = currentTarget?.port ? chips.get(currentTarget.port) : undefined;
    if (currentTarget?.port && currentTarget.fqbn && chipHere?.mcu) {
      const have = await mcuOfFqbn(currentTarget.fqbn);
      if (have && have !== chipHere.mcu) {
        const old = currentTarget.fqbn;
        const fqbn = deduped.find((c) => c.port === currentTarget!.port)?.fqbn ?? null;
        currentTarget = { port: currentTarget.port, fqbn, userChosen: false };
        output.appendLine(`[Grease] ${currentTarget.port} holds an ${chipHere.chip}, not ${have}: target ${old} → ${fqbn ?? "none"}`);
        void vscode.window.showInformationMessage(
          `Grease: ${currentTarget.port} holds an ${chipHere.chip}. Board set to ${fqbn ?? "none (pick one)"}.`,
        );
        await saveTarget(context, currentTarget);
        await syncTargetToServer(currentTarget, { chipFix: true });
      }
    }
    if (!currentTarget?.userChosen && candidatesWithFqbn.length === 1) {
      const only = candidatesWithFqbn[0];
      const remembered = only.fromMemory;
      currentTarget = { port: only.port, fqbn: only.fqbn!, userChosen: false };
      if (remembered) {
        const logKey2 = `auto:${only.fqbn}:${only.port}`;
        if (!loggedRestores.has(logKey2)) {
          output.appendLine(`[BoardMemory] Restored ${only.fqbn} for ${vidPidKey(only.vid, only.pid) ?? only.port}`);
          loggedRestores.add(logKey2);
        }
      }
      await saveTarget(context, currentTarget);
      await syncTargetToServer(currentTarget, { auto: true });
    } else if (
      !currentTarget?.userChosen &&
      candidatesWithFqbn.length === 0 &&
      portsOnly.length === 1
    ) {
      const onlyPort = portsOnly[0];
      currentTarget = { port: onlyPort.port, fqbn: null };
      await saveTarget(context, currentTarget);
      await syncTargetToServer(currentTarget, { auto: true });
    }
    if (!currentTarget) {
      if (!deduped.some((c) => !!c.vid)) {
        setWarning("No board connected");
        if (changed) output.appendLine("No Arduino port detected.");
      } else {
        setWarning("Select board/port");
      }
    } else {
      setOk(currentTarget);
      // Only for a board that is plugged in: during the port-gone grace the
      // target is kept, but it must not be announced or get a core install.
      const targetHere = !!currentTarget.port && portsNow.has(currentTarget.port);
      if (changed && targetHere) {
        output.appendLine(
          `[Grease] ${currentTarget.port} · ${currentTarget.fqbn ?? "unknown board"}`,
        );
        // Missing core: install the Supported Core this board needs, once per plug-in.
        const cand = deduped.find((c) => c.port === currentTarget?.port);
        const needed = extractCoreFromFqbn(currentTarget.fqbn) ?? cand?.neededCore ?? null;
        if (needed) void installMissingCore(needed, currentTarget.port);
      }
    }
    refreshToolbarState();
  };

  // One scan at a time: the 5 s poll, the /dev watcher and AI Tether's port
  // changes all ask for one. A request during a scan runs once after it.
  let reconcileRunning: Promise<void> | null = null;
  let reconcileAgain = false;
  const reconcileTarget = async (): Promise<void> => {
    if (reconcileRunning) {
      reconcileAgain = true;
      return reconcileRunning;
    }
    reconcileRunning = (async () => {
      try {
        do {
          reconcileAgain = false;
          await reconcileTargetOnce();
        } while (reconcileAgain);
      } finally {
        reconcileRunning = null;
      }
    })();
    return reconcileRunning;
  };

  /** Show the QuickPick board selector and persist whatever the user picks. */
  const refreshPortsAndBoard = async (): Promise<void> => {
    if (isPickerOpen) return;
    isPickerOpen = true;
    output.show(true);
    try {
      const detection = await refreshBoards(output);
      lastCandidates = detection.candidates;
      const picked = await promptForTarget(context, lastCandidates, currentTarget);
      if (picked) {
        currentTarget = { ...picked, userChosen: true };
        await syncTargetToServer(currentTarget);
        setOk(currentTarget);
        refreshToolbarState();
      }
    } finally {
      isPickerOpen = false;
    }
  };

  /** Resolve a valid sketch folder or offer to start a new sketch. */
  const getValidSketchPath = async (): Promise<string | null> => {
    const sketchPath = getSketchFolder();
    if (!sketchPath) return null;
    const mainIno = findMainSketchFile(sketchPath);
    if (mainIno) return sketchPath;
    output.appendLine(`Invalid sketch folder (main .ino missing): ${sketchPath}`);
    const choice = await vscode.window.showWarningMessage(
      "Grease: Current folder is not a valid sketch (missing main .ino).",
      "Start Sketch",
      "Cancel",
    );
    if (choice === "Start Sketch") {
      await vscode.commands.executeCommand("arduinoMcp.startSketch");
    }
    return null;
  };

  /**
   * Resolve the sketch the user wants to build, staged into a folder that
   * arduino-cli will accept.
   *
   * Priority (matches user intent — "compile the file in front of me"):
   *   1. The .ino in the active editor (or any visible editor). If its parent
   *      folder name doesn't match the .ino basename — the common loose-.ino
   *      case — copy it plus sibling .h/.cpp into a temp folder of the right
   *      shape and use that.
   *   2. Fall back to folder-based detection (workspace root / subfolder
   *      sketch) for the legacy "no editor open but workspace IS a sketch"
   *      case.
   *
   * Returns null if no sketch is resolvable; the caller surfaces an error.
   */
  const resolveSketchForBuild = async (): Promise<PreparedSketch | null> => {
    const activeIno = getActiveInoPath();
    if (activeIno) {
      const prepared = prepareSketchForBuild(activeIno);
      if (prepared) return prepared;
      output.appendLine(`[compile] Could not stage active sketch: ${activeIno}`);
    }
    const folder = await getValidSketchPath();
    if (!folder) return null;
    const mainIno = findMainSketchFile(folder);
    if (!mainIno) return null;
    return { sketchDir: folder, mainIno, isTemp: false };
  };

  /**
   * After a failed compile, arduino-cli prints error paths pointing into our
   * temp staging dir (e.g. `/tmp/arduino-grease-sketches/X/X/X.ino:42`). Map
   * those back to the user's real file so the squiggles land on the code
   * they're editing.
   */
  const remapTempPathToOriginal = (filePath: string, prepared: PreparedSketch): string => {
    if (!prepared.isTemp || !prepared.originalDir) return filePath;
    const normalizedTemp = path.normalize(filePath);
    const normalizedSketchDir = path.normalize(prepared.sketchDir);
    if (!normalizedTemp.startsWith(normalizedSketchDir)) return filePath;
    const rel = path.relative(normalizedSketchDir, normalizedTemp);
    // The main .ino lives at `<sketchDir>/<base>.ino` but the user's file may
    // have a different basename — remap that one specifically.
    if (
      prepared.originalIno &&
      path.basename(rel).toLowerCase() === path.basename(prepared.mainIno).toLowerCase() &&
      path.dirname(rel) === "."
    ) {
      return prepared.originalIno;
    }
    return path.join(prepared.originalDir, rel);
  };

  /**
   * Compile a sketch (no upload). On success we ALSO regenerate
   * compile_commands.json so clangd picks up any changes. On failure we parse
   * arduino-cli's stderr into VS Code Diagnostics so red squiggles appear on
   * the offending lines.
   */
  const runCompileOnly = async (
    fqbnOverride?: string,
  ): Promise<{
    ok: boolean;
    sketchPath?: string;
    fqbn?: string;
    prepared?: PreparedSketch;
  }> => {
    output.show(true);
    const prepared = await resolveSketchForBuild();
    if (!prepared) {
      vscode.window.showErrorMessage(
        "Grease: No .ino file open or valid sketch folder found.",
      );
      return { ok: false };
    }
    const sketchPath = prepared.sketchDir;
    const fqbn = fqbnOverride ?? currentTarget?.fqbn ?? null;
    if (!fqbn) {
      const choice = await vscode.window.showWarningMessage(
        "Grease: Board unknown. Install/select a core so an FQBN is available.",
        "Install core...",
        "Select board/port...",
      );
      if (choice === "Install core...") {
        await vscode.commands.executeCommand("arduinoMcp.installCore");
      } else {
        await vscode.commands.executeCommand("arduinoMcp.refreshPortsBoards");
      }
      return { ok: false };
    }
    // Log the user-facing path (the .ino they're editing), not the temp copy.
    const displayPath = prepared.isTemp ? prepared.originalIno! : sketchPath;
    output.appendLine(`Compiling "${displayPath}"...`);
    if (prepared.isTemp) {
      output.appendLine(
        `  (staged into temp sketch folder: ${sketchPath} — original folder name '${path.basename(
          prepared.originalDir!,
        )}' doesn't match .ino basename '${path.basename(prepared.mainIno, ".ino")}')`,
      );
    }
    const compileStatus = vscode.window.setStatusBarMessage("$(sync~spin) Compiling...");
    let res: { ok: boolean; stdout: string; stderr: string };
    try {
      res = await postCompile(sketchPath);
    } catch {
      compileStatus.dispose();
      vscode.window.showErrorMessage("Grease: Compile failed — server unreachable.");
      return { ok: false };
    }
    compileStatus.dispose();
    output.appendLine(res.stdout ?? "");
    output.appendLine(res.stderr ?? "");
    if (!res.ok) {
      // Parse `file:line:col: error: message` into VS Code Diagnostics.
      const diagMap = new Map<string, vscode.Diagnostic[]>();
      const errorRegex = /^(.+):([0-9]+):([0-9]+):\s*(error|warning):\s*(.+)$/gm;
      let m: RegExpExecArray | null;
      const combinedOutput = res.stdout + "\n" + res.stderr;
      while ((m = errorRegex.exec(combinedOutput)) !== null) {
        const filePath = remapTempPathToOriginal(m[1], prepared);
        const line = Math.max(0, parseInt(m[2], 10) - 1);
        const col = Math.max(0, parseInt(m[3], 10) - 1);
        const severity =
          m[4] === "error"
            ? vscode.DiagnosticSeverity.Error
            : vscode.DiagnosticSeverity.Warning;
        const message = m[5];
        const range = new vscode.Range(line, col, line, col + 1);
        const diag = new vscode.Diagnostic(range, message, severity);
        diag.source = "Grease";
        const uri = vscode.Uri.file(filePath);
        const key = uri.toString();
        if (!diagMap.has(key)) diagMap.set(key, []);
        diagMap.get(key)!.push(diag);
      }
      arduinoDiagnostics.clear();
      for (const [uriStr, diags] of diagMap) {
        arduinoDiagnostics.set(vscode.Uri.parse(uriStr), diags);
      }
      vscode.window.showErrorMessage("Grease: Compile failed (see Output).");
      return { ok: false, prepared };
    }
    arduinoDiagnostics.clear();
    vscode.window.setStatusBarMessage("$(check) Compile OK", 4000);
    return { ok: true, sketchPath, fqbn, prepared };
  };

  /**
   * Compile (unless skipped) then upload. Saves all dirty files first so
   * what's compiled matches what's on screen.
   */
  const runUpload = async (opts?: {
    verifyFirst?: boolean;
    fqbnOverride?: string;
    portOverride?: string;
  }): Promise<boolean> => {
    output.show(true);
    await vscode.workspace.saveAll(false);
    let prepared: PreparedSketch | null = null;
    let fqbn: string | null = opts?.fqbnOverride ?? currentTarget?.fqbn ?? null;
    if (opts?.verifyFirst !== false) {
      const compiled = await runCompileOnly(fqbn ?? undefined);
      if (!compiled.ok) return false;
      prepared = compiled.prepared ?? null;
      fqbn = compiled.fqbn ?? fqbn;
    } else {
      prepared = await resolveSketchForBuild();
    }
    if (!prepared) {
      vscode.window.showErrorMessage("Grease: No .ino file to upload.");
      return false;
    }
    const sketchPath = prepared.sketchDir;
    const port = opts?.portOverride ?? currentTarget?.port ?? null;
    if (!port) {
      vscode.window.showWarningMessage("Grease: Select a port first.");
      await vscode.commands.executeCommand("arduinoMcp.refreshPortsBoards");
      return false;
    }
    if (!fqbn) {
      vscode.window.showWarningMessage("Grease: Select a board first.");
      await vscode.commands.executeCommand("arduinoMcp.refreshPortsBoards");
      return false;
    }
    const displayPath = prepared.isTemp ? prepared.originalIno! : sketchPath;
    output.appendLine(`Uploading "${displayPath}"...`);
    const uploadStatus = vscode.window.setStatusBarMessage("$(sync~spin) Uploading...");
    let res: { ok: boolean; stdout: string; stderr: string };
    try {
      res = await postUpload(sketchPath);
    } catch {
      uploadStatus.dispose();
      vscode.window.showErrorMessage("Grease: Upload failed — server unreachable.");
      return false;
    }
    uploadStatus.dispose();
    output.appendLine(res.stdout ?? "");
    output.appendLine(res.stderr ?? "");
    toolbar.view?.webview.postMessage({ type: "uploadResult", success: res.ok });
    if (!res.ok) {
      const combined = (res.stdout ?? "") + (res.stderr ?? "");
      const isBootloaderError =
        combined.includes("butterfly_recv") ||
        combined.includes("initialization failed") ||
        combined.includes("avrdude: stk500");
      if (isBootloaderError) {
        vscode.window
          .showErrorMessage(
            "Upload failed: board not in bootloader mode. For Leonardo/32U4 boards: double-tap RESET, then upload within 8 s.",
            "Recovery Upload",
          )
          .then((choice) => {
            if (choice === "Recovery Upload")
              void vscode.commands.executeCommand("arduinoMcp.recoveryUpload");
          });
      } else {
        vscode.window.showErrorMessage("Grease: Upload failed (see Output).");
      }
      toolbar.view?.webview.postMessage({ type: "uploadResult", success: false });
      return false;
    }
    vscode.window.setStatusBarMessage("$(check) Upload OK", 4000);
    return true;
  };

  /** Bootloader-burn flow for bare chips; programmer is optional. */
  const runFirmwareUpload = async (fqbn: string, port: string): Promise<void> => {
    output.show(true);
    const programmer = await vscode.window.showInputBox({
      title: "Programmer",
      prompt: "Enter programmer (e.g., avrispmkii, usbtinyisp) or leave empty for default",
      ignoreFocusOut: false,
    });
    if (programmer === undefined) return;
    output.appendLine(`[Mngrs] Burning bootloader for ${fqbn} on ${port}...`);
    const args = ["burn-bootloader", "-b", fqbn, "-p", port];
    if (programmer) {
      args.push("-P", programmer);
    }
    try { await signalThrust(); } catch { /* server may be starting */ }
    const up = explainCliFailure(await runArduinoCli(args, undefined, TIMEOUTS.upload));
    try { await signalIdle(); } catch { /* ignore */ }
    output.appendLine(up.stdout);
    output.appendLine(up.stderr);
    if (!up.success) {
      vscode.window.showErrorMessage("Grease: Burn bootloader failed (see Output).");
      return;
    }
    vscode.window.showInformationMessage("Grease: Bootloader burned to target.");
  };

  // ── Initial reconciliation + health checks ──────────────────────────────
  //    Not awaited: the commands below must register right away, or sidebar
  //    buttons fail with "command not found" while the first board scan runs
  //    (up to 30 s on a slow USB hub).
  let healthBusy = false;
  cliReady = prepareArduinoCli();
  void (async () => {
    await reconcileTarget();
    await delay(3000);
    await checkServerHealth(true);
    const healthInterval = setInterval(() => {
      if (healthBusy) return;
      healthBusy = true;
      void checkServerHealth(false).finally(() => (healthBusy = false));
    }, 500);
    context.subscriptions.push({ dispose: () => clearInterval(healthInterval) });
  })();

  // Board detection by events (ADR 0014): arduino-cli's serial-discovery
  // reports every USB serial port that appears or disappears, on every OS,
  // so the board list runs only when a port changes. No poll: if the tool is
  // unavailable, the Refresh button scans.
  context.subscriptions.push(
    startSerialDiscovery({
      output,
      onChange: (why) => {
        output.appendLine(`[Grease] ports: ${why} — rescanning boards.`);
        void reconcileTarget();
      },
      isUploading: () => !!lastServerState?.uploading,
    }),
  );

  // ── COMMAND REGISTRATIONS ────────────────────────────────────────────────
  // Each command below is exposed in the Command Palette and (mostly) wired
  // to a button in the sidebar toolbar. The comment above each one explains
  // WHAT the command does — useful when reading the source to learn.

  context.subscriptions.push(
    /**
     * `arduinoMcp.serverStatus`
     * Print the current state of the bundled MCP server to the Output panel.
     * Useful when you're debugging "is my AI client actually talking to me?".
     */
    vscode.commands.registerCommand("arduinoMcp.serverStatus", async () => {
      output.show(true);
      output.appendLine("Checking MCP server status...");
      if (!serverProcess) {
        output.appendLine("Server status: stopped");
        serverHealthy = false;
        refreshToolbarState();
        return;
      }
      await checkServerHealth(true);
    }),

    /**
     * `arduinoMcp.refreshServer`
     * Smart cycle: starts the server if stopped; restarts it if running but
     * unhealthy; stops it if running and healthy. Mapped to the ||R|| link
     * next to "AI Tether" in the sidebar.
     */
    vscode.commands.registerCommand("arduinoMcp.refreshServer", async () => {
      output.show(true);
      if (!serverProcess) {
        await startServer();
        await delay(1000);
        await checkServerHealth(true);
        return;
      }
      if (serverProcess && !serverHealthy) {
        await stopServer();
        await delay(1000);
        await startServer();
        await delay(1000);
        await checkServerHealth(true);
        return;
      }
      if (serverProcess && serverHealthy) {
        await stopServer();
        refreshToolbarState();
      }
    }),

    /** `arduinoMcp.startServer` — explicit start. */
    vscode.commands.registerCommand("arduinoMcp.startServer", async () => {
      output.show(true);
      const ok = await startServer();
      if (ok) {
        await checkServerHealth(true);
        output.appendLine("Server start command completed.");
      }
      refreshToolbarState();
    }),

    /** `arduinoMcp.stopServer` — explicit stop. */
    vscode.commands.registerCommand("arduinoMcp.stopServer", async () => {
      output.show(true);
      await stopServer();
      refreshToolbarState();
    }),

    /** `arduinoMcp.toggleServer` — start ↔ stop in one keystroke. */
    vscode.commands.registerCommand("arduinoMcp.toggleServer", async () => {
      output.show(true);
      if (serverProcess) {
        await stopServer();
      } else {
        await startServer();
        await checkServerHealth(true);
      }
      refreshToolbarState();
    }),

    /**
     * `arduinoMcp.startSketch`
     * Prompt for a name + parent folder, then call `arduino-cli sketch new`
     * to create a properly-structured sketch folder, and open the new
     * `.ino` in the editor.
     */
    vscode.commands.registerCommand("arduinoMcp.startSketch", async () => {
      output.show(true);
      const name = await vscode.window.showInputBox({
        title: "Start new sketch",
        prompt: "Sketch name",
        placeHolder: "BlinkNano33",
        ignoreFocusOut: false,
        validateInput: (value) => {
          const trimmed = value.trim();
          if (!trimmed) return "Sketch name is required.";
          if (!/^[A-Za-z0-9_\-]+$/.test(trimmed))
            return "Use only letters, numbers, underscore, or dash.";
          return null;
        },
      });
      if (!name) return;
      const defaultParent = vscode.Uri.file(SKETCHBOOK_DIR);
      const pickedFolder = await vscode.window.showOpenDialog({
        canSelectFiles: false,
        canSelectFolders: true,
        canSelectMany: false,
        defaultUri: defaultParent,
        openLabel: "Create Sketch Here",
        title: "Choose parent folder",
      });
      if (!pickedFolder?.[0]) return;
      const parent = pickedFolder[0].fsPath;
      const sketchPath = path.join(parent, name.trim());
      output.appendLine(`$ arduino-cli sketch new "${sketchPath}"`);
      const res = await runArduinoCli(["sketch", "new", sketchPath]);
      output.appendLine(res.stdout);
      output.appendLine(res.stderr);
      if (!res.success) {
        vscode.window.showErrorMessage("Grease: Failed to create sketch. See output.");
        return;
      }
      const inoPath = path.join(sketchPath, `${name.trim()}.ino`);
      try {
        const doc = await vscode.workspace.openTextDocument(vscode.Uri.file(inoPath));
        await vscode.window.showTextDocument(doc, { preview: false });
      } catch (e) {
        output.appendLine(
          `Could not open sketch file automatically: ${e instanceof Error ? e.message : String(e)}`,
        );
      }
      output.appendLine(`Sketch created: ${sketchPath}`);
      vscode.window.showInformationMessage(`Grease: Sketch created (${name.trim()}).`);
    }),

    /**
     * `arduinoMcp.installCore`
     * Quick-pick a common core (AVR, SAMD, Mbed, ESP32, RP2040) and run
     * `arduino-cli core install`. After install, reconcile so the new
     * board becomes pickable in the QuickPick.
     */
    vscode.commands.registerCommand("arduinoMcp.installCore", async () => {
      const picked = await vscode.window.showQuickPick(
        SUPPORTED_CORES.map((c) => ({ label: c.name, description: c.id, detail: c.boards.join(" · "), id: c.id })),
        { title: "Install board core (online)", placeHolder: "Pick a Supported Core" },
      );
      if (!picked) return;
      if (await installCoreWithProgress(picked.id)) await reconcileTarget();
    }),

    /**
     * `arduinoMcp.verify`
     * Compile the current sketch without uploading. Also refreshes
     * compile_commands.json for clangd on success.
     */
    vscode.commands.registerCommand("arduinoMcp.verify", async () => {
      const res = await runCompileOnly();
      toolbar.view?.webview.postMessage({ type: "verifyResult", success: res.ok === true });
    }),

    /**
     * `arduinoMcp.upload`
     * Compile-then-upload the current sketch to the selected board+port.
     * Bound to F5 / Shift+Enter on `.ino` files (see package.json keybindings).
     */
    vscode.commands.registerCommand("arduinoMcp.upload", async () => {
      const ok = await runUpload({ verifyFirst: true });
      toolbar.view?.webview.postMessage({ type: "uploadResult", success: ok === true });
    }),

    /**
     * `arduinoMcp.refreshPortsBoards`
     * Show the QuickPick board/port selector. Mapped to ||R|| in the sidebar.
     */
    vscode.commands.registerCommand("arduinoMcp.refreshPortsBoards", async () => {
      await refreshPortsAndBoard();
    }),

    /** `arduinoMcp.selectTarget` — alias for refreshPortsBoards. */
    vscode.commands.registerCommand("arduinoMcp.selectTarget", async () => {
      await refreshPortsAndBoard();
    }),

    /**
     * `arduinoMcp.toggleSerial`
     * Open the serial monitor if disconnected, close it if connected.
     */
    vscode.commands.registerCommand("arduinoMcp.toggleSerial", async () => {
      if (serialMonitorPanel.isConnected) {
        await serialMonitorPanel.stop();
      } else {
        await vscode.commands.executeCommand("arduinoMcp.openSerialMonitor");
      }
      refreshToolbarState();
    }),

    /**
     * `arduinoMcp.openSerialMonitor`
     * Start the serial monitor at 9600 baud on the currently selected port
     * and print a help line of console verbs (baud=, send=, clear,
     * disconnect, connect).
     */
    vscode.commands.registerCommand("arduinoMcp.openSerialMonitor", async () => {
      output.show(true);
      const defaultPort = currentTarget?.port ?? lastCandidates[0]?.port ?? null;
      await serialMonitorPanel.start(defaultPort, 9600);
      output.appendLine(
        `Type "baud=X" to set a new baud rate, or try any of the following commands: 'send="Hello World"', "clear", "disconnect", or "connect". Transmitting from ${defaultPort ?? "(unknown port)"} below:`,
      );
    }),

    /**
     * `arduinoMcp.serialCommand`
     * Prompt for a single serial-monitor command (without opening a new
     * monitor session). Useful for one-off `send=...` calls from the
     * Command Palette.
     */
    vscode.commands.registerCommand("arduinoMcp.serialCommand", async () => {
      output.show(true);
      const command = await vscode.window.showInputBox({
        title: "Serial command",
        prompt: "Enter serial command",
        placeHolder: 'baud=9600 | send="Hello World" | clear | disconnect | connect',
        ignoreFocusOut: false,
      });
      if (!command) return;
      await serialMonitorPanel.handleConsoleCommand(command);
    }),

    /**
     * `arduinoMcp.openSerialPlotter`
     * Open the canvas-based serial plotter in a webview, auto-connect to
     * the current port, and move it to a new VS Code window so it can sit
     * beside the editor.
     */
    vscode.commands.registerCommand("arduinoMcp.openSerialPlotter", async () => {
      output.show(true);
      const defaultPort = currentTarget?.port ?? lastCandidates[0]?.port ?? null;
      if (serialMonitorPanel.isConnected) {
        await serialMonitorPanel.stop();
        output.appendLine("Serial disconnected (Plotter opened).");
      }
      serialPlotterPanel.show(defaultPort, true);
      setTimeout(() => {
        void vscode.commands.executeCommand("workbench.action.moveEditorToNewWindow");
      }, 500);
      refreshToolbarState();
    }),

    /**
     * `arduinoMcp.openExamples`
     * Open the standalone Examples webview. The serial monitor is closed
     * first so it can't conflict with the panel.
     */
    vscode.commands.registerCommand("arduinoMcp.openExamples", async () => {
      output.show(true);
      if (serialMonitorPanel.isConnected) {
        await serialMonitorPanel.stop();
        output.appendLine("Serial disconnected (Examples panel opened).");
        refreshToolbarState();
      }
      examplesPanel.show(currentTarget?.fqbn ?? null);
    }),

    /**
     * `arduinoMcp.openGreasePanel`
     * Show the Help panel: setup checks, AI Tether, the agents on this
     * computer and the Grease Skill. It replaces the AI Prompt tab.
     */
    vscode.commands.registerCommand("arduinoMcp.openGreasePanel", async () => {
      toolbar.showTab("grease");
      await vscode.commands.executeCommand(`${ArduinoToolbarViewProvider.viewType}.focus`);
    }),

    /**
     * `arduinoMcp.reinstallSkill`
     * Write the Grease Skill again everywhere it belongs. A copy the
     * Developer edited is saved as SKILL.md.bak first.
     */
    vscode.commands.registerCommand("arduinoMcp.reinstallSkill", async () => {
      await context.globalState.update(SKILL_REMOVED_KEY, false);
      const results = installSkill(SKILL_TEXT, { force: true });
      for (const r of results) output.appendLine(`[Grease] Skill ${r.action}: ${r.file}${r.backup ? ` (your copy: ${r.backup})` : ""}`);
      const failed = results.filter((r) => r.action === "error");
      const saved = results.filter((r) => r.backup).map((r) => r.backup);
      if (failed.length) {
        void vscode.window.showErrorMessage(`Grease: the skill was not written to ${failed.map((r) => r.file).join(", ")}: ${failed[0].error}`);
      } else {
        void vscode.window.showInformationMessage(
          `Grease: the Grease Skill is installed for ${results.map((r) => r.readers).join("; ")}.${saved.length ? ` Your edited copy is saved as ${saved.join(", ")}.` : ""} Restart your agent if /grease does not show up.`,
        );
      }
      toolbar.postGrease();
    }),

    /**
     * `arduinoMcp.removeSkill`
     * Delete the skill files Grease wrote. Copies the Developer edited stay.
     */
    vscode.commands.registerCommand("arduinoMcp.removeSkill", async () => {
      const ok = await vscode.window.showWarningMessage(
        "Remove the Grease Skill from your agents?",
        { modal: true, detail: "Grease deletes only the skill files it wrote. /grease stops working until you press Reinstall skill." },
        "Remove",
      );
      if (ok !== "Remove") return;
      await context.globalState.update(SKILL_REMOVED_KEY, true);
      const results = removeSkill();
      for (const r of results) output.appendLine(`[Grease] Skill ${r.action}: ${r.file}`);
      const kept = results.filter((r) => r.action === "kept-edited");
      void vscode.window.showInformationMessage(
        kept.length
          ? `Grease: removed. Kept your edited copy: ${kept.map((r) => r.file).join(", ")}.`
          : "Grease: the Grease Skill was removed. Reinstall skill in Help puts it back.",
      );
      toolbar.postGrease();
    }),

    /** `arduinoMcp.showSkill` — open the installed skill (or the bundled one) read-only. */
    vscode.commands.registerCommand("arduinoMcp.showSkill", async () => {
      const installed = skillStatus(SKILL_TEXT).find((t) => t.state !== "missing");
      const doc = installed
        ? await vscode.workspace.openTextDocument(vscode.Uri.file(installed.file))
        : await vscode.workspace.openTextDocument({ language: "markdown", content: SKILL_TEXT });
      await vscode.window.showTextDocument(doc, { preview: true });
      if (installed) await vscode.commands.executeCommand("workbench.action.files.setActiveEditorReadonlyInSession");
    }),

    /** `arduinoMcp.addAgentTools` — pick agents and add the Grease tools to them. */
    vscode.commands.registerCommand("arduinoMcp.addAgentTools", async () => {
      await offerAgentTools(true);
    }),

    /**
     * `arduinoMcp.cycleAccentColor`
     * Walk through 7 dark accent colors for the status bar / focus border /
     * activity bar badges. Workspace-scoped if a workspace is open,
     * otherwise global.
     */
    vscode.commands.registerCommand("arduinoMcp.cycleAccentColor", async () => {
      const ACCENT_COLORS = [
        "#005FA0",
        "#6B0000",
        "#A34300",
        "#8F6809",
        "#6B004A",
        "#520A85",
        "#004D00",
      ];
      const config = vscode.workspace.getConfiguration("workbench");
      const current = (config.get("colorCustomizations") as Record<string, string>) || {};
      const currentColor = current["statusBar.background"] || "#007ACC";
      const idx = ACCENT_COLORS.indexOf(currentColor);
      const nextIdx = (idx + 1) % ACCENT_COLORS.length;
      const color = ACCENT_COLORS[nextIdx];
      const updated = {
        ...current,
        "statusBar.background": color,
        "statusBar.noFolderBackground": color,
        "statusBar.debuggingBackground": color,
        "statusBarItem.remoteBackground": color,
        focusBorder: color,
        "activityBarBadge.background": color,
        "panelTitle.activeBorder": color,
      };
      const target =
        vscode.workspace.workspaceFolders && vscode.workspace.workspaceFolders.length > 0
          ? vscode.ConfigurationTarget.Workspace
          : vscode.ConfigurationTarget.Global;
      await config.update("colorCustomizations", updated, target);
      output.appendLine(
        `Accent color cycled to ${color} (Target: ${target === vscode.ConfigurationTarget.Workspace ? "Workspace" : "Global"})`,
      );
      refreshToolbarState();
      toolbar.view?.webview.postMessage({ type: "accentColor", color });
    }),

    /**
     * `arduinoMcp.recoveryUpload`
     * Recovers a bricked board in bootloader mode. The user must first pick an
     * FQBN in the Managers panel (Choose as Target), then click this button and
     * double-tap RESET on the board. Watches 10s for any new port to appear,
     * uploads a hardcoded blink sketch to unblock the board, and updates the
     * target with the recovered port.
     */
    vscode.commands.registerCommand("arduinoMcp.recoveryUpload", async () => {
      if (!currentTarget?.fqbn) {
        vscode.window.showWarningMessage(
          "Grease: Pick a board from the Managers panel first.",
        );
        return;
      }
      const fqbn = currentTarget.fqbn;

      const tmpDir = path.join(os.tmpdir(), "grease-recovery", "Blink");
      fs.mkdirSync(tmpDir, { recursive: true });
      fs.writeFileSync(
        path.join(tmpDir, "Blink.ino"),
        "void setup() { pinMode(LED_BUILTIN, OUTPUT); }\n" +
          "void loop() {\n" +
          "  digitalWrite(LED_BUILTIN, HIGH); delay(500);\n" +
          "  digitalWrite(LED_BUILTIN, LOW);  delay(500);\n" +
          "}\n",
      );

      vscode.window.showInformationMessage(
        "Grease: Double-tap RESET on your board now. Watching 10 seconds for bootloader port...",
      );
      output.appendLine("[Recovery] Watching for bootloader port (10s)...");

      const knownPorts = new Set(lastCandidates.map((c) => c.port).filter(Boolean));
      const WATCH_MS = 10_000;
      const POLL_MS = 500;
      const deadline = Date.now() + WATCH_MS;
      let recoveryPort: string | null = null;

      while (Date.now() < deadline) {
        await delay(POLL_MS);
        const detection = await refreshBoards(output);
        const newPort = detection.candidates
          .map((c) => c.port)
          .find((p): p is string => typeof p === "string" && !knownPorts.has(p));
        if (newPort) {
          recoveryPort = newPort;
          break;
        }
      }

      if (!recoveryPort) {
        vscode.window.showWarningMessage(
          "Grease: No bootloader port appeared within 10 seconds.",
        );
        output.appendLine("[Recovery] Timed out — no new port detected.");
        return;
      }

      output.appendLine(`[Recovery] Bootloader port found: ${recoveryPort}. Uploading blink sketch...`);
      try { await signalThrust(); } catch { /* server may be starting */ }

      const up = explainCliFailure(
        await runArduinoCli(["upload", "-b", fqbn, "-p", recoveryPort, tmpDir], undefined, TIMEOUTS.upload),
      );

      try { await signalIdle(); } catch { /* ignore */ }
      output.appendLine(up.stdout);
      output.appendLine(up.stderr);

      if (!up.success) {
        vscode.window.showErrorMessage("Grease: Recovery upload failed (see Output).");
        return;
      }

      currentTarget = { port: recoveryPort, fqbn, userChosen: true };
      await saveTarget(context, currentTarget);
      await syncTargetToServer(currentTarget);
      setOk(currentTarget);
      refreshToolbarState();

      // Save board memory so future reconnects are transparent
      await delay(1500);
      const postRecovery = await refreshBoards(output);
      lastCandidates = postRecovery.candidates;
      const recoveredCandidate = postRecovery.candidates.find((c) => c.port === recoveryPort);
      if (recoveredCandidate) {
        const keys = boardMemoryKeys(recoveredCandidate);
        if (keys.length > 0) {
          const mem = await loadBoardMemory(context);
          for (const k of keys) mem[k] = { fqbn };
          await saveBoardMemory(context, mem);
          output.appendLine(`[Recovery] BoardMemory saved (${keys.join(", ")}) → ${fqbn}`);
        }
      }

      vscode.window.showInformationMessage(
        `Grease: Recovery successful. Board is live on ${recoveryPort}.`,
      );
    }),
  );

  refreshToolbarState();
}

export function deactivate(): void {
  // Resources are cleaned up via the subscriptions array.
}
