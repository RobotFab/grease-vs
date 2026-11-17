/**
 * views/toolbarView.ts
 * ────────────────────
 * Grease VS sidebar — the dark-themed "command center" the user
 * spends most of their time in. Implemented as a single VS Code WebviewView
 * (the kind that docks in the activity bar) with four tab panels:
 *
 *   • Board   — Port/FQBN status, action buttons (Start Sketch, Compile,
 *               Upload, Serial, Plotter, Examples, Managers, /grease),
 *               and the MCP "AI Tether" toggle/health indicator.
 *   • Mngrs   — Embedded Boards & Libraries manager tabs.
 *   • X-mpls  — Example sketches browser (filterable by text/library).
 *   • /grease — Setup checks (arduino-cli, Node.js), AI Tether, the agents
 *               found on this computer, and the Grease Skill.
 *
 * The webview communicates with the extension via `postMessage`:
 *   webview → extension: { type: 'cmd', command }      (run a VS Code command)
 *                        { type: 'updateIndexes' }     (refresh boards/libs)
 *                        { type: 'boardSearch', query }
 *                        { type: 'chooseTarget', fqbn }
 *                        { type: 'libInstallSelected', name }
 *                        { type: 'libInstallGit', input }
 *                        { type: 'boardCatalogInstall', id }
 *                        { type: 'greaseRefresh' | 'gAddTools' | ... }  (/grease)
 *                        ...etc
 *   extension → webview: { type: 'state', state }      (port/fqbn/health/...)
 *                        { type: 'boards' | 'libraries' | 'examples', rows }
 *                        { type: 'greaseStatus', status }
 *                        { type: 'bubble' | 'uploadResult' | 'verifyResult' }
 *                        { type: 'serialLine', line } | { type: 'motion', on }
 *
 * Bubble-up: glyphs rise through the panel behind the text. Exhaust while a
 * compile or upload runs (amber when an agent runs it), serial lines rise as
 * words, red glyphs fall on an error, white ones burst out of ||Upload|| on
 * success. Links rebound them; Grease-green text lets them through. The
 * `arduinoMcp.motion` setting (and the OS reduced-motion switch) stops it.
 */

import * as vscode from "vscode";
import * as fs from "node:fs";
import * as path from "node:path";
import { runArduinoCli, explainCliFailure, TIMEOUTS } from "../arduinoCli";
import {
  installGitHubLibrary,
  installRegistryLibrary,
  installSupportedCore,
  updateIndexes,
} from "../../server/lib/install.mjs";
import { SUPPORTED_CORES, parseGitHubLibrary } from "../../server/lib/supported.mjs";
import { writeGreaseSettings } from "../../server/lib/cli.mjs";
import { AGENT_IDS, addTools } from "../../server/lib/agents.mjs";
import { bubbleOf } from "../../server/lib/bubbleState.mjs";
import { linuxChecks, type LinuxCheck } from "../../server/lib/linuxChecks.mjs";
import { agentRows, checkArduinoCli, checkNode, type CliCheck, type NodeCheck, type SetupJob } from "../greaseSetup";
import { asRows, uniqueRows, bundledSnippetRows, type ExampleRow } from "../ui/examplesPanel";
import { parseInstalledBoards, parseLibraries } from "../ui/managersPanel";

/** Snapshot of extension state the webview reads to render itself. */
export interface ToolbarState {
  port: string | null;
  fqbn: string | null;
  connectedPorts: string[];
  serverRunning: boolean;
  serverHealthy: boolean;
  lastScanAtMs: number | null;
  serialActive: boolean;
  uploading?: boolean;
  accentColor?: string;
  /** arduino-cli is missing: the Board tab shows "Setup needed". */
  setupNeeded?: boolean;
  tetherPort?: number | null;
}

/** What the Bubble-up animation shows; AI Tether's /state decides it. */
export type BubbleState = "idle" | "compile" | "upload" | "agent" | "agentCompile" | "agentUpload";

/** Bubble-up state from AI Tether's /state: who runs the compile or upload, or an agent at work. */
export { bubbleOf };

/** Callbacks the extension provides to the toolbar (so the panel can mutate extension state). */
export interface ToolbarActions {
  chooseTarget: (fqbn: string) => Promise<void>;
  uploadFirmwareToTarget: (fqbn: string) => Promise<void>;
  recoveryUpload: () => Promise<void>;
  serialOff: () => Promise<void>;
  cycleAccent: (color: string) => Promise<void>;
  /** Called after a library install completes successfully. */
  onLibraryInstalled?: () => Promise<void>;
  /** Called after a board core is installed via the Managers tab — triggers immediate re-detection. */
  onBoardInstalled?: () => Promise<void>;
  /** Called after the Help panel checks arduino-cli. */
  onSetupChecked?: (cliFound: boolean) => void;
  /** Help › Install arduino-cli or Retry: install arduino-cli if missing, then its first run. */
  prepareCli?: () => Promise<boolean>;
}

export class ArduinoToolbarViewProvider implements vscode.WebviewViewProvider {
  static readonly viewType = "arduinoMcp.toolbar";
  view: vscode.WebviewView | null = null;

  private state: ToolbarState = {
    port: null,
    fqbn: null,
    connectedPorts: [],
    serverRunning: false,
    serverHealthy: false,
    lastScanAtMs: null,
    serialActive: false,
  };

  /** Last /grease check results; null until the first check runs. */
  private lastCli: CliCheck | null = null;
  private lastNode: NodeCheck | null = null;
  private lastLinux: LinuxCheck[] = [];
  private job: SetupJob | null = null;
  /** Tab to open when the view is created later (the Help command). */
  private pendingTab: string | null = null;
  private bubble: BubbleState = "idle";

  constructor(
    private context: vscode.ExtensionContext,
    private output: vscode.OutputChannel,
    private actions: ToolbarActions,
  ) {}

  setState(next: ToolbarState): void {
    this.state = next;
    this.postState();
  }

  /** Bubble-up state; posted only when it changes. */
  setBubble(next: BubbleState): void {
    if (next === this.bubble) return;
    this.bubble = next;
    void this.view?.webview.postMessage({ type: "bubble", state: next });
  }

  /** The newest serial line; it rises through the panel as a word. */
  postSerialLine(line: string): void {
    void this.view?.webview.postMessage({ type: "serialLine", line: line.slice(0, 48) });
  }

  private postMotion(): void {
    const on = vscode.workspace.getConfiguration("arduinoMcp").get<boolean>("motion", true);
    void this.view?.webview.postMessage({ type: "motion", on });
  }

  /** Switch the sidebar to a tab, now or as soon as the view is created. */
  showTab(tab: "board" | "grease"): void {
    if (this.view) void this.view.webview.postMessage({ type: "switchTab", tab });
    else this.pendingTab = tab;
  }

  resolveWebviewView(view: vscode.WebviewView): void {
    this.view = view;
    view.title = "";
    view.webview.options = {
      enableScripts: true,
      localResourceRoots: [vscode.Uri.joinPath(this.context.extensionUri, "resources")],
    };
    view.webview.html = this.html(view.webview);
    view.webview.onDidReceiveMessage((msg) => void this.onMessage(msg));
    const motionSub = vscode.workspace.onDidChangeConfiguration((e) => {
      if (e.affectsConfiguration("arduinoMcp.motion")) this.postMotion();
    });
    view.onDidDispose(() => motionSub.dispose());
    this.postState();
    this.postMotion();
    void view.webview.postMessage({ type: "bubble", state: this.bubble });
  }

  private postState(): void {
    this.view?.webview.postMessage({ type: "state", state: this.state });
  }

  /** Re-render the Help panel with the latest checks and agent rows. */
  postGrease(): void {
    this.view?.webview.postMessage({
      type: "greaseStatus",
      status: {
        cli: this.lastCli,
        job: this.job,
        node: this.lastNode,
        linux: this.lastLinux,
        agents: agentRows(vscode.env.appName),
        tetherPort: this.state.serverRunning ? (this.state.tetherPort ?? null) : null,
      },
    });
  }

  /** The install running now (or the last one that failed); null clears it. */
  setJob(job: SetupJob | null): void {
    this.job = job;
    this.postGrease();
  }

  /** Check arduino-cli again and re-render the Help panel. */
  refreshCli(): void {
    void this.checkCli().then(() => this.postGrease());
  }

  private async checkCli(): Promise<void> {
    this.lastCli = await checkArduinoCli();
    this.actions.onSetupChecked?.(this.lastCli.state !== "missing");
  }

  /** Help panel messages; true when handled. */
  private async onGreaseMessage(msg: any): Promise<boolean> {
    switch (msg.type) {
      case "greaseRefresh":
        this.postGrease();
        if (msg.checks || !this.lastCli || !this.lastNode) {
          await Promise.all([
            this.checkCli(),
            checkNode(process.execPath).then((n) => (this.lastNode = n)),
            linuxChecks().then((l) => (this.lastLinux = l)),
          ]);
          this.postGrease();
        }
        return true;
      case "gPrepareCli":
        await this.actions.prepareCli?.();
        await this.checkCli();
        this.postGrease();
        return true;
      case "gUseCli": {
        // Only a copy the check itself offered; never a path typed into the webview.
        const picked = String(msg.path ?? "");
        if (!(this.lastCli?.copies ?? []).includes(picked)) return true;
        writeGreaseSettings({ arduinoCliPath: picked });
        this.output.appendLine(`[Setup] Using arduino-cli at ${picked}.`);
        await this.checkCli();
        this.postGrease();
        return true;
      }
      case "gCopy":
        await vscode.env.clipboard.writeText(String(msg.text ?? "").slice(0, 2000));
        vscode.window.setStatusBarMessage("Grease: copied", 2000);
        return true;
      case "gAddTools": {
        const id = String(msg.id ?? "");
        if (!AGENT_IDS.includes(id)) return true;
        const r = await addTools(id, { editorNode: process.execPath });
        this.output.appendLine(`[Grease] Add tools (${id}): ${r.message}${r.manual ? ` ${r.manual}` : ""}`);
        if (r.ok) {
          void vscode.window.showInformationMessage(`Grease: ${r.message}`);
        } else if (r.manual) {
          const copy = await vscode.window.showWarningMessage(`Grease: ${r.message} ${r.manual}`, "Copy command");
          if (copy) await vscode.env.clipboard.writeText(r.manual);
        } else {
          void vscode.window.showWarningMessage(`Grease: ${r.message}`);
        }
        this.postGrease();
        return true;
      }
      case "gReinstall":
        await vscode.commands.executeCommand("arduinoMcp.reinstallSkill");
        this.postGrease();
        return true;
      case "gRemove":
        await vscode.commands.executeCommand("arduinoMcp.removeSkill");
        this.postGrease();
        return true;
      case "gShow":
        await vscode.commands.executeCommand("arduinoMcp.showSkill");
        return true;
      case "gOpen": {
        // Download links come from Grease's own agent table, never from the webview.
        const row = agentRows(vscode.env.appName).find((r) => r.id === String(msg.id ?? ""));
        if (row) await vscode.env.openExternal(vscode.Uri.parse(row.download));
        return true;
      }
    }
    return false;
  }

  /** Dispatcher for messages arriving from the webview script. */
  private async onMessage(msg: any): Promise<void> {
    if (!msg?.type) return;

    try {
      if (await this.onGreaseMessage(msg)) return;
    } catch (e) {
      const err = e instanceof Error ? e.message : String(e);
      this.output.appendLine(`[Grease] Error: ${err}`);
      this.view?.webview.postMessage({ type: "greaseError", error: err });
      return;
    }

    // "Run one of Grease's own commands" pipe. Only arduinoMcp.* commands
    // pass, so text injected into the webview cannot reach other commands.
    if (msg.type === "cmd") {
      const command = String(msg.command ?? "");
      if (!/^arduinoMcp\.[A-Za-z]+$/.test(command)) return;
      await vscode.commands.executeCommand(command);
      return;
    }

    // The examples tab inside the toolbar reuses the standalone panel's helpers.
    if (msg.type === "list") {
      try {
        const argsBase = ["lib", "examples", "--json"];
        const base = await runArduinoCli(argsBase);
        if (!base.success)
          throw new Error(base.stderr || base.stdout || "Failed to list examples");
        let rows: ExampleRow[] = asRows(JSON.parse(base.stdout));
        const fqbn = this.state.fqbn;
        if (fqbn) {
          const b = await runArduinoCli(["lib", "examples", "--fqbn", fqbn, "--json"]);
          if (b.success) rows = rows.concat(asRows(JSON.parse(b.stdout)));
        }
        rows = rows.concat(bundledSnippetRows(this.context.extensionPath));
        this.view?.webview.postMessage({ type: "examples", rows: uniqueRows(rows) });
      } catch (e) {
        this.view?.webview.postMessage({
          type: "exError",
          error: e instanceof Error ? e.message : String(e),
        });
      }
      return;
    }

    if (msg.type === "openExample") {
      const target = String(msg.path ?? "").trim();
      if (!target) return;
      let inoFile: string | null = null;
      // Support direct .ino paths (bundled snippets) and folders (arduino-cli examples)
      if (target.toLowerCase().endsWith(".ino") && fs.existsSync(target)) {
        inoFile = target;
      } else {
        const name = path.basename(target);
        const preferred = path.join(target, name + ".ino");
        if (fs.existsSync(preferred)) {
          inoFile = preferred;
        } else {
          try {
            const files = fs.readdirSync(target).filter((f) => f.toLowerCase().endsWith(".ino"));
            if (files.length > 0) inoFile = path.join(target, files[0]);
          } catch {
            /* ignore */
          }
        }
      }
      if (!inoFile) {
        vscode.window.showWarningMessage("Grease: No .ino file found in this example.");
        return;
      }
      const doc = await vscode.workspace.openTextDocument(vscode.Uri.file(inoFile));
      await vscode.window.showTextDocument(doc, { preview: false });
      await vscode.commands.executeCommand("workbench.action.files.setActiveEditorReadonlyInSession");
      vscode.window
        .showInformationMessage(
          "This is a read-only example. Save a copy to edit it.",
          "Save As New Sketch",
        )
        .then((choice) => {
          if (choice === "Save As New Sketch")
            vscode.commands.executeCommand("workbench.action.files.saveAs");
        });
      return;
    }

    const post = (payload: any): void => void this.view?.webview.postMessage(payload);
    try {
      if (msg.type === "updateIndexes" || msg.type === "boardsInstalled") {
        if (msg.type === "updateIndexes") {
          // Online Action: refresh the Supported Core indexes and the library index.
          this.output.appendLine("[Mngrs] Updating board and library indexes (online)...");
          const up = await updateIndexes({ onData: (d) => this.output.append(d) });
          if (!up.success) {
            const bad = explainCliFailure(up.core.success ? up.lib : up.core);
            this.output.appendLine(bad.stderr);
            post({ type: "mgrError", error: bad.stderr.split("\n").filter(Boolean).slice(-2).join(" ") });
          }
        }
        const boards = await runArduinoCli(["core", "list", "--json"]);
        if (boards.success) {
          const rows = parseInstalledBoards(boards.stdout);
          post({ type: "boards", rows });
          this.output.appendLine(`[Mngrs] Loaded ${rows.length} installed boards.`);
        } else {
          post({ type: "mgrError", error: "arduino-cli core list failed. See Output › Grease VS." });
        }
      } else if (msg.type === "boardSearch") {
        const boards = await runArduinoCli(["core", "list", "--json"]);
        if (!boards.success) {
          post({ type: "mgrError", error: "arduino-cli core list failed. See Output › Grease VS." });
          return;
        }
        post({
          type: "boards",
          rows: parseInstalledBoards(boards.stdout),
          query: String(msg.query ?? ""),
        });
      } else if (msg.type === "chooseTarget") {
        const fqbn = String(msg.fqbn ?? "").trim();
        if (!fqbn) {
          post({ type: "mgrError", error: "Select a board first." });
          return;
        }
        this.output.appendLine(`[Mngrs] Choosing target ${fqbn}...`);
        await this.actions.chooseTarget(fqbn);
      } else if (msg.type === "uploadFirmwareToTarget") {
        const fqbn = String(msg.fqbn ?? "").trim();
        if (!fqbn) {
          post({ type: "mgrError", error: "Select a board first." });
          return;
        }
        this.output.appendLine(`[Mngrs] Upload firmware to target ${fqbn}...`);
        await this.actions.uploadFirmwareToTarget(fqbn);
      } else if (msg.type === "recoveryUpload") {
        await this.actions.recoveryUpload();
      } else if (msg.type === "libList") {
        const res = await runArduinoCli(["lib", "list", "--json"]);
        if (!res.success) {
          post({ type: "mgrError", error: res.stderr || res.stdout });
          return;
        }
        post({ type: "libraries", rows: parseLibraries(res.stdout) });
      } else if (msg.type === "libSearch") {
        const q = String(msg.query ?? "").trim();
        // Online Action: lib search refreshes the library index when it is old.
        const res = explainCliFailure(
          await runArduinoCli(["lib", "search", ...(q ? [q] : []), "--json"], undefined, TIMEOUTS.index),
        );
        if (!res.success) {
          post({ type: "mgrError", error: res.stderr || res.stdout });
          return;
        }
        post({ type: "libraries", rows: parseLibraries(res.stdout) });
      } else if (msg.type === "libInstallSelected") {
        const name = String(msg.name ?? "").trim();
        if (!name) {
          post({ type: "mgrError", error: "Select a library first." });
          return;
        }
        this.output.appendLine(`[Mngrs] Installing library ${name} (online)...`);
        const res = explainCliFailure(
          await installRegistryLibrary(name, null, { onData: (d) => this.output.append(d) }),
        );
        if (!res.success) {
          this.output.appendLine(res.stderr);
          post({ type: "mgrError", error: res.stderr || res.stdout });
        } else {
          vscode.window.showInformationMessage(`Grease: Library ${name} installed.`);
          if (this.actions.onLibraryInstalled) {
            await this.actions.onLibraryInstalled();
          }
        }
      } else if (msg.type === "toggleSerial") {
        await vscode.commands.executeCommand("arduinoMcp.toggleSerial");
      } else if (msg.type === "libInstallGit") {
        const input = String(msg.input ?? "").trim();
        if (!input) return;
        const parsed = parseGitHubLibrary(input);
        if ("error" in parsed) {
          post({ type: "mgrError", error: parsed.error });
          return;
        }
        // Online Action, Developer only: show exactly what will be cloned.
        const ok = await vscode.window.showWarningMessage(
          `Install the library at ${parsed.url}?`,
          {
            modal: true,
            detail:
              "Grease downloads this repository from GitHub and compiles its code into your sketches. Install only code you trust.",
          },
          "Install",
        );
        if (ok !== "Install") return;
        this.output.appendLine(`[Mngrs] Installing library from GitHub (online): ${parsed.url}`);
        const res = explainCliFailure(
          await installGitHubLibrary(input, { onData: (d) => this.output.append(d) }),
        );
        if (!res.success) {
          this.output.appendLine(res.stderr);
          post({ type: "mgrError", error: res.stderr || res.stdout });
        } else {
          post({ type: "gitInstalled" });
          vscode.window.showInformationMessage(`Grease: Library ${parsed.owner}/${parsed.repo} installed.`);
          if (this.actions.onLibraryInstalled) {
            await this.actions.onLibraryInstalled();
          }
        }
      } else if (msg.type === "boardCatalogInstall") {
        // The webview sends only an id; the URL comes from the Supported Core list.
        const entry = SUPPORTED_CORES.find((c) => c.id === String(msg.id ?? ""));
        if (!entry) {
          post({ type: "mgrError", error: "Pick a board from the list." });
          return;
        }
        this.output.appendLine(`[Mngrs] Installing ${entry.name} (${entry.id}, online)...`);
        const resCore = explainCliFailure(
          await installSupportedCore(entry.id, { onData: (d) => this.output.append(d) }),
        );
        if (!resCore.success) {
          this.output.appendLine(resCore.stderr);
          post({ type: "mgrError", error: resCore.stderr || resCore.stdout });
        } else {
          vscode.window.showInformationMessage(`Grease: ${entry.name} installed.`);
          const boards2 = await runArduinoCli(["core", "list", "--json"]);
          if (boards2.success)
            post({ type: "boards", rows: parseInstalledBoards(boards2.stdout) });
          if (this.actions.onBoardInstalled) await this.actions.onBoardInstalled();
        }
      } else if (msg.type === "serialOff") {
        await this.actions.serialOff();
      } else if (msg.type === "setMotion") {
        // The eye writes the same setting as Settings, so both always agree.
        await vscode.workspace.getConfiguration("arduinoMcp").update("motion", msg.on !== false, vscode.ConfigurationTarget.Global);
      } else if (msg.type === "cycleAccent") {
        const color = String(msg.color ?? "#007ACC");
        await this.actions.cycleAccent(color);
      }
    } catch (e) {
      const err = e instanceof Error ? e.message : String(e);
      this.output.appendLine(`[Mngrs] Error: ${err}`);
      post({ type: "mgrError", error: err });
    }
  }

  private html(webview: vscode.Webview): string {
    const iconUri = webview.asWebviewUri(
      vscode.Uri.joinPath(this.context.extensionUri, "resources", "icon.png"),
    );
    // Supported Cores for the "Install new board" list; "<" escaped so the JSON cannot close the script tag.
    const catalogJson = JSON.stringify(
      SUPPORTED_CORES.map(({ id, name, vendor, boards }) => ({ id, name, vendor, boards })),
    ).replace(/</g, "\\u003c");
    const initialTab = JSON.stringify(this.pendingTab ?? "board");
    this.pendingTab = null;
    // No remote scripts, styles, fonts or connections; inline handlers stay until the Help panel rewrite.
    const csp = [
      "default-src 'none'",
      `img-src ${webview.cspSource} data:`,
      `style-src ${webview.cspSource} 'unsafe-inline'`,
      `font-src ${webview.cspSource}`,
      "script-src 'unsafe-inline'",
    ].join("; ");
    return `<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="Content-Security-Policy" content="${csp}">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  :root { --muted: #4a9960; --ink: #c8ffd8; --bg: #080e0a; --panel: #0b120d; --stroke: #1a3322; --mono: Consolas, Menlo, Monaco, 'Courier New', monospace; }
  body { background: var(--bg); color: var(--ink); font-family: var(--mono); font-size: 11px; height: 100vh; overflow: hidden; }
  .panel-area { width: 100%; height: 100vh; display: flex; flex-direction: column; background: var(--bg); overflow: hidden; position: relative; }
  canvas.bubble { position: absolute; top: 0; left: 0; width: 100%; height: 100%; pointer-events: none; z-index: 0; }
  .panel-content { position: relative; z-index: 1; padding: 10px 10px 4px; overflow-y: auto; height: 100%; }
  .tab-panel { display: none; }
  .mgr-err { color: #ff8a7a; font-size: 10px; white-space: pre-wrap; margin: 4px 0; }
  .mgr-err:empty { display: none; }
  .tab-panel.active { display: block; }
  a { font-family: var(--mono); font-size: 11px; text-decoration: none; cursor: pointer; background: none; border: none; padding: 0; margin: 0; display: inline; }
  a:hover { text-decoration: underline; }
  a.dim { color: var(--muted); } a.dim:hover { color: var(--ink); }
  a.c-green { color: #55efc4; } a.c-amber { color: #f6c542; } a.c-teal { color: #4ecdc4; }
  a.c-sky { color: #81ecec; } a.c-purple { color: #a29bfe; } a.c-blue { color: #74b9ff; }
  a.c-coral { color: #ff6b6b; } a.c-pink { color: #fd79a8; } a.c-std { color: var(--ink); }
  .act-row a { transition: color .2s, text-shadow .25s; }
  a.hot { color: #fff !important; text-shadow: 0 0 8px currentColor; }
  a.hot.pulse { text-shadow: 0 0 16px #fff, 0 0 4px #fff; transition: text-shadow 0s; }
  .serverStatus.agent-on { color: #ffb142 !important; }
  .ascii-line { font-family: var(--mono); font-size: 11px; color: var(--ink); white-space: pre; line-height: 1.6; display: block; overflow: hidden; text-overflow: ellipsis; }
  .dim { color: var(--muted); }
  .actions-wrap { margin-top: 6px; }
  .act-row { padding: 1px 0 1px 2px; font-family: var(--mono); font-size: 10px; line-height: 1.8; white-space: pre; }
  .server-section { margin-top: 12px; font-family: var(--mono); font-size: 11px; color: var(--ink); }
  .server-line { display: flex; align-items: center; gap: 6px; margin-top: 3px; }
  .sblock { font-size: 16px; line-height: 1; cursor: pointer; }
  /* Eye: stops the moving glyphs in one click (the old Grease chip). Off = struck through, solid dark green. */
  .eye { display: inline-flex; align-items: center; padding: 2px 4px; background: rgba(0,0,0,0.35); border: 1px solid var(--stroke); border-radius: 3px; cursor: pointer; opacity: 0.55; transition: opacity 140ms, background 140ms, border-color 140ms; }
  .eye:hover, .eye:focus-visible { opacity: 1; }
  .eye svg { display: block; }
  .eye .o { fill: none; stroke: var(--ink); stroke-width: 1.4; }
  .eye .p { fill: var(--ink); }
  .eye .s { stroke: var(--ink); stroke-width: 1.6; display: none; }
  .eye.off { opacity: 1; background: #10402a; border-color: #1d6b45; }
  .eye.off .o { stroke: #4fbf87; } .eye.off .p { fill: #4fbf87; } .eye.off .s { display: inline; stroke: #4fbf87; }
  .logo-area { margin-top: 12px; }
  .logo-img { display: block; width: 132px; opacity: 0.18; filter: brightness(2) saturate(0.3); }
  .logo-caption { font-family: var(--mono); font-size: 10px; color: #5bc8f5; margin-top: 3px; letter-spacing: 0.04em; }
  .logo-ascii { font-family: var(--mono); font-size: 9px; color: var(--muted); white-space: pre; line-height: 1.4; margin-top: 6px; opacity: 0.55; }

  .mgr-header { display: flex; align-items: baseline; justify-content: space-between; margin-bottom: 10px; }
  .mgr-label { color: var(--muted); font-size: 10px; }
  .mgr-section { margin-bottom: 10px; }
  .mgr-section-title { color: var(--muted); font-size: 10px; text-transform: uppercase; letter-spacing: 0.05em; margin-bottom: 4px; }
  .mgr-card { border: 1px solid var(--stroke); background: var(--panel); border-radius: 6px; padding: 8px; }
  .mgr-card input { background: #050d07; color: var(--ink); border: 1px solid var(--stroke); border-radius: 4px; padding: 4px 6px; font-size: 11px; font-family: var(--mono); width: 100%; margin-bottom: 6px; }
  .mgr-card input:focus { outline: none; border-color: var(--muted); }
  .mgr-top-row { margin-bottom: 7px; font-family: var(--mono); font-size: 10px; display: flex; gap: 14px; flex-wrap: wrap; }
  .mgr-list { max-height: 160px; overflow-y: auto; }
  .mgr-item { padding: 5px 6px; border: 1px solid var(--stroke); border-radius: 4px; margin-bottom: 3px; cursor: pointer; font-size: 10px; background: #050d07; }
  .mgr-item:hover { border-color: #58aa58; }
  .mgr-item .name { color: var(--ink); font-weight: bold; }
  .mgr-item .meta { color: var(--muted); }
  .setup-needed { display: none; margin-top: 4px; font-size: 10px; }
  .g-row { display: flex; gap: 6px; align-items: baseline; flex-wrap: wrap; margin-bottom: 3px; }
  .g-ok { color: #55efc4; } .g-warn { color: #f6c542; } .g-bad { color: #ff8a7a; }
  .g-path { color: var(--muted); word-break: break-all; }
  .g-code { display: block; background: #050d07; border: 1px solid var(--stroke); border-radius: 4px; padding: 4px 6px; margin: 3px 0; color: var(--ink); white-space: pre-wrap; word-break: break-all; user-select: all; }
  .g-note { color: var(--muted); margin-top: 3px; }
  .g-verdict { font-size: 13px; margin-bottom: 6px; }
  .g-hint { font-size: 11px; margin-top: 2px; }
  .g-line { justify-content: space-between; flex-wrap: nowrap; }
  .g-toggle { display: inline-block; margin-top: 6px; }
  #gDetails { margin-top: 8px; }
  #gAgents { margin-top: 8px; }
  .g-foot { margin-top: 8px; margin-bottom: 0; }
</style>
</head>
<body>
<div class="panel-area" id="panelArea">
    <canvas class="bubble" id="bubbleCanvas"></canvas>
    <div class="panel-content">

      <div class="tab-panel active" id="panel-board">
        <span class="ascii-line"><span class="dim">+------------ -  -   +</span></span>
        <span class="ascii-line"><span class="dim">| Port:     </span><a class="dim" href="#" onclick="cmd('arduinoMcp.refreshPortsBoards');return false">||R||</a></span>
        <span class="ascii-line"><span class="dim">| </span><span id="portVal">?</span></span>
        <span class="ascii-line"><span class="dim">| Board:</span></span>
        <span class="ascii-line"><span class="dim">| </span><span id="fqbnVal">?</span></span>
        <span class="ascii-line"><span class="dim">+   -   -  - --------+</span></span>
        <div id="setupNeeded" class="setup-needed"><a class="c-coral" href="#" onclick="switchTab('grease');return false">||Setup needed: open Help||</a></div>
        <div class="actions-wrap">
          <div class="act-row"><span class="dim">+-------------- -  -   +</span></div>
          <div class="act-row"> <a class="c-teal"  href="#" onclick="cmd('arduinoMcp.startSketch');return false">||Start Sketch||</a></div>
          <div class="act-row"> <a id="uploadBtn" class="c-coral"  href="#" onclick="onUploadClick();return false">||Upload||</a></div>
          <div class="act-row"> <a id="compileBtn" class="c-teal"   href="#" onclick="cmd('arduinoMcp.verify');return false">||Compile||</a></div>
          <div class="act-row"> <a id="serialBtn" class="c-blue" href="#" onclick="toggleSerial();return false">||Serial||</a></div>
          <div class="act-row"> <a class="c-teal" href="#" onclick="cmd('arduinoMcp.openSerialPlotter');return false">||Plotter||</a></div>
          <div class="act-row"> <a class="c-blue"   href="#" onclick="switchTab('examples');return false">||Examples||</a></div>
          <div class="act-row"> <a class="c-teal"  href="#" onclick="switchTab('managers');return false">||Managers||</a></div>
          <div class="act-row"> <a class="c-purple"   href="#" onclick="switchTab('grease');return false">||Help||</a></div>
          <div class="act-row"><span class="dim">+   -   -  - ----------+</span></div>
        </div>
        <div class="server-section">
          AI Tether <a class="dim" href="#" onclick="cmd('arduinoMcp.refreshServer');return false">||R||</a>
          <div class="server-line">
            <span class="sblock serverBlock" style="color:#49c06b" onclick="cmd('arduinoMcp.toggleServer')">&#x2588;</span>
            <span style="color:var(--muted);font-size:10px" class="serverStatus">running</span>
          </div>
          <div class="server-line" style="margin-top:6px">
            <span class="sblock" id="accentBlock" style="color:#007ACC;cursor:pointer;font-size:16px;line-height:1" onclick="cycleAccent()" title="Click to cycle accent color">&#x2588;</span>
            <span class="eye" id="eyeBtn" role="button" tabindex="0" aria-pressed="false" aria-label="Moving glyphs" title="Hide the moving glyphs" onclick="flipMotion()" onkeydown="if(event.key==='Enter'||event.key===' '){event.preventDefault();flipMotion();}">
              <svg viewBox="0 0 24 16" width="18" height="12" aria-hidden="true"><path class="o" d="M1 8 C5 2, 19 2, 23 8 C19 14, 5 14, 1 8 Z"/><circle class="p" cx="12" cy="8" r="3"/><line class="s" x1="3" y1="14" x2="21" y2="2"/></svg>
            </span>
          </div>
        </div>
        <div class="logo-area">
          <img class="logo-img" id="logoImg" src="${iconUri}" draggable="false" />
        </div>
      </div>

      <div class="tab-panel" id="panel-managers">
        <div class="mgr-header">
          <span class="mgr-label"><a class="c-coral" href="#" onclick="switchTab('board');return false" style="margin-right:4px">||R||</a>Mngrs</span>
          <a class="c-std" id="updateIdx" href="#" title="Online: downloads the board and library indexes" onclick="event.preventDefault()">||Update indexes &#8599;||</a>
        </div>
        <div id="mgrErr" class="mgr-err" role="alert"></div>
        <div class="mgr-section">
          <div class="mgr-section-title">Board</div>
          <div class="mgr-card">
            <div class="mgr-top-row">
              <a class="c-std" id="chooseTargetBtn" href="#" onclick="event.preventDefault()">||Choose as target||</a>
              <a class="c-coral" id="recoveryUploadBtn" href="#" onclick="event.preventDefault()">||Recovery Upload||</a>
            </div>
            <input id="boardQuery" class="boardQuery" placeholder="arduino, esp32, rp2040..." />
            <div class="mgr-list" id="boardsList"></div>
            <a class="c-amber" id="installBoardBtn" href="#" title="Online: downloads the board core" onclick="event.preventDefault()" style="font-size:10px;margin-top:6px;display:block">||Install new board &#8599;||</a>
          </div>
        </div>
        <div class="mgr-section">
          <div class="mgr-section-title">Library</div>
          <div class="mgr-card">
            <div class="mgr-top-row">
              <a class="c-std" id="libListBtn" href="#" onclick="event.preventDefault()">||List installed||</a>
              <a class="c-green" id="libInstallBtn" href="#" title="Online: downloads the library" onclick="event.preventDefault()">||Install library &#8599;||</a>
            </div>
            <input id="libQuery" class="libQuery" placeholder="search online: servo, imu..." />
            <div class="mgr-list" id="libsList"></div>
          </div>
        </div>
        <div class="mgr-card" style="margin-top: 9px;">
          <div class="muted" style="font-size: 10px; margin-bottom: 4px;">
            Install library from GitHub &#8599; (owner/repo, a github.com link, or owner/repo#tag)
          </div>
          <input id="libGitInput" placeholder="pololu/romi32u4-arduino-library" onkeydown="if(event.key==='Enter') { vscode.postMessage({type:'libInstallGit', input:this.value}); }" />
        </div>
      </div>

      <div class="tab-panel" id="panel-examples">
        <div class="mgr-header">
          <span class="mgr-label"><a class="c-coral" href="#" onclick="switchTab('board');return false" style="margin-right:4px">||R||</a>X-mpls</span>
          <a class="c-std" href="#" onclick="vscode.postMessage({type:'list',library:''});return false">&#8635;</a>
        </div>
        <div class="mgr-card" style="margin-bottom:6px">
          <div style="display:flex;align-items:flex-end;gap:6px;flex-wrap:wrap">
            <div style="flex:1;min-width:70px">
              <div class="muted" style="font-size:9px;margin-bottom:2px">Search Text</div>
              <input id="exQuery" placeholder="blink, imu, wifi" oninput="renderEx()" onkeydown="if(event.key==='Enter')renderEx()" />
            </div>
            <div style="flex:1;min-width:70px">
              <div class="muted" style="font-size:9px;margin-bottom:2px">Library name</div>
              <input id="exLibrary" placeholder="wire, servo" oninput="renderEx()" onkeydown="if(event.key==='Enter')renderEx()" />
            </div>
          </div>
        </div>
        <div id="exError" style="color:#ffb4b4;white-space:pre-wrap;font-size:10px;margin-bottom:4px"></div>
        <div id="exCount" class="muted" style="font-size:9px;margin-bottom:4px">loading...</div>
        <div id="exOut" class="mgr-list" style="max-height:416px"></div>
      </div>

      <div class="tab-panel" id="panel-grease">
        <div class="mgr-header">
          <span class="mgr-label"><a class="c-coral" href="#" onclick="switchTab('board');return false" style="margin-right:4px">||R||</a>Help</span>
          <a class="c-std" href="#" title="Check everything again" onclick="greaseRefresh(true);return false">&#8635;</a>
        </div>
        <div id="gErr" class="mgr-err" role="alert"></div>

        <div class="g-verdict" id="gVerdict"><span class="dim">checking...</span></div>
        <div id="gFix"></div>
        <a class="c-std g-toggle" href="#" id="gToggle" data-toggle>&#9662; Details</a>

        <div id="gDetails">
          <div id="gSetup"></div>
          <div class="g-row g-line">
            <span><span class="sblock serverBlock" style="color:#49c06b" onclick="cmd('arduinoMcp.toggleServer')">&#x2588;</span> AI Tether <span style="color:var(--muted);font-size:10px" class="serverStatus">running</span></span>
            <span class="g-path" id="gPort"></span>
          </div>
          <div id="gAgents"></div>
          <div class="mgr-top-row g-foot">
          <a class="c-green" href="#" onclick="vscode.postMessage({type:'gReinstall'});return false">||Reinstall skill||</a>
          <a class="c-coral" href="#" onclick="vscode.postMessage({type:'gRemove'});return false">||Remove skill||</a>
          <a class="c-std" href="#" onclick="vscode.postMessage({type:'gShow'});return false">||Show skill||</a>
          </div>
        </div>
      </div>

    </div>
</div>

<script>
const vscode = acquireVsCodeApi();

function cmd(command) { vscode.postMessage({ type: 'cmd', command: command }); }

function switchTab(panel) {
  document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
  const panelEl = document.getElementById('panel-' + panel);
  if (panelEl) panelEl.classList.add('active');
  queueGather();
  if (panel === 'managers') { vscode.postMessage({type:'serialOff'}); vscode.postMessage({type:'boardsInstalled'}); vscode.postMessage({type:'libList'}); }
  if (panel === 'examples') { vscode.postMessage({type:'serialOff'}); vscode.postMessage({type:'list',library:''}); }
  if (panel === 'grease') greaseRefresh(false);
}

const ACCENT_COLORS = ['#005FA0','#6B0000','#A34300','#8F6809','#6B004A','#520A85','#004D00'];
let accentIndex = 0;
function cycleAccent() {
  accentIndex = (accentIndex + 1) % ACCENT_COLORS.length;
  const color = ACCENT_COLORS[accentIndex];
  const block = document.getElementById('accentBlock');
  if (block) block.style.color = color;
  vscode.postMessage({ type: 'cycleAccent', color: color });
}

// ── Bubble-up ──────────────────────────────────────────────────────────────
// Glyphs rise from the bottom of the panel. Exhaust while a compile or upload
// runs (amber when an agent runs it), serial lines rise as words, red glyphs
// fall on an error. Glyphs rebound off links and pass through Grease-green
// text at 8 px.
const bubbleCanvas = document.getElementById('bubbleCanvas');
const bctx = bubbleCanvas.getContext('2d');
const panelArea = document.getElementById('panelArea');
const panelContent = document.querySelector('.panel-content');
const CHARS = 'abcdefghijklmnopqrstuvwxyzNH01+=-./\\\\[]{}()?!<>:;'.split('');
const AMBER = ['#ffb142', '#ffa502', '#f6b93b', '#fdcb6e', '#e1a233', '#ffbe76'];
const RED = ['#ff4757', '#ff6b6b', '#d63031', '#ee5253'];
const SERIAL_GREEN = '#2f7d50';
const rnd = (a, b) => a + Math.random() * (b - a);
const pick = (arr) => arr[(Math.random() * arr.length) | 0];
const pickCh = () => pick(CHARS);
const CHARW = 7, LIFE = 40000;
// Devin's narrowest side bar. The exhaust column sits at its middle.
const MIN_W = 170;
// Compile is half of Upload: half the glyphs on screen, half the speed. Slow glyphs
// stay on screen about twice as long, so Compile emits 0.3 of the glyphs.
const EXHAUST = { compile: 0.9, upload: 3, agentCompile: 0.9, agentUpload: 3 };
const SPEED = { compile: 0.5, upload: 1, agentCompile: 0.5, agentUpload: 1 };
const FLOATER_MS = { idle: 2200, agent: 700 };
const RISE = 1.8, THRUST_RISE = 2.2;
const POP_MS = 2000, POP_R = 140, ERROR_MS = 5000;
// At most this many glyphs, the oldest dropped first; Upload with Serial on sits near it.
const MAX_GLYPHS = 900;
// Text rectangles read from the open tab; glyphs look them up by 16-px row band.
const MAX_SOLIDS = 600, BAND = 16;
// Glyphs shrink and dim while they pass through Grease-green text (as in Rust Grease's bubble.js).
const FONT = 'bold 12px Consolas, Menlo, monospace', FONT_FLOW = 'bold 8px Consolas, Menlo, monospace';
const AGENT = new Set(['agent', 'agentCompile', 'agentUpload']);

// state: what is drawn; server: the last state AI Tether reported (an error covers it for 5 s).
const B = { state: 'idle', server: 'idle', serial: false, motion: true };
let BW = 0, BH = 0, solids = [], bands = new Map(), textBottom = 0, parts = [], queued = null;
let lastLinePrint = 0, lastSyn = 0, exhaustAcc = 0, bubbleErrorTimer = null, gatherQueued = false;
const mouse = { x: -999, y: -999 };
panelArea.addEventListener('mousemove', (e) => { const r = panelArea.getBoundingClientRect(); mouse.x = e.clientX - r.left; mouse.y = e.clientY - r.top; });
panelArea.addEventListener('mouseleave', () => { mouse.x = mouse.y = -999; });

function glyphColor(p) {
  if (B.state === 'error') return p.red || (p.red = pick(RED));
  if (p.line) return SERIAL_GREEN;
  if (AGENT.has(B.state)) return p.amber || (p.amber = pick(AMBER));
  if (B.state === 'compile' || B.state === 'upload') return p.kind === 'syn' ? '#55efc4' : '#5fcf93';
  return p.kind === 'syn' ? '#7af0b5' : '#5fcf93';
}

/** Grease green = hue 90–160° with some saturation; everything else rebounds. */
function isGreaseGreen(cssColor) {
  const m = cssColor.match(/\\d+(\\.\\d+)?/g); if (!m) return false;
  const [r, g, b] = m.slice(0, 3).map((x) => Number(x) / 255);
  const max = Math.max(r, g, b), min = Math.min(r, g, b), d = max - min;
  if (max === 0 || d / max < 0.15) return false; // white or grey
  let h = max === r ? ((g - b) / d) % 6 : max === g ? (b - r) / d + 2 : (r - g) / d + 4;
  h = (h * 60 + 360) % 360;
  return h >= 90 && h <= 160;
}

function resizeBubble() {
  const r = panelArea.getBoundingClientRect(), dpr = window.devicePixelRatio || 1;
  BW = r.width || 300; BH = r.height || 660;
  bubbleCanvas.width = Math.round(BW * dpr); bubbleCanvas.height = Math.round(BH * dpr);
  bctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  gather();
}
function queueGather() {
  if (gatherQueued) return;
  gatherQueued = true;
  requestAnimationFrame(gather);
}
/** Rectangles of the visible text in the open tab: links rebound, Grease-green text lets glyphs through. */
function gather() {
  gatherQueued = false;
  solids = []; bands = new Map(); textBottom = 0;
  const panel = document.querySelector('.tab-panel.active');
  if (!panel) return;
  const r0 = panelArea.getBoundingClientRect(), outer = panelContent.getBoundingClientRect();
  const walker = document.createTreeWalker(panel, NodeFilter.SHOW_TEXT);
  const range = document.createRange(), bounceOf = new Map();
  for (let n = walker.nextNode(), seen = 0; n && seen < 4 * MAX_SOLIDS && solids.length < MAX_SOLIDS; n = walker.nextNode(), seen++) {
    const first = n.data.search(/\\S/); if (first < 0) continue;
    const el = n.parentElement; if (!el) continue;
    let bounce = bounceOf.get(el);
    if (bounce === undefined) { bounce = !isGreaseGreen(getComputedStyle(el).color); bounceOf.set(el, bounce); }
    const list = el.closest('.mgr-list'), lr = list ? list.getBoundingClientRect() : outer;
    const top = Math.max(outer.top, lr.top), bottom = Math.min(outer.bottom, lr.bottom);
    range.setStart(n, first); range.setEnd(n, n.data.replace(/\\s+$/, '').length);
    for (const r of range.getClientRects()) {
      if (r.width < 1 || r.bottom <= top || r.top >= bottom) continue;
      const s = { l: r.left - r0.left - 1.5, r: r.right - r0.left + 1.5, t: Math.max(r.top, top) - r0.top - 2, b: Math.min(r.bottom, bottom) - r0.top + 2, bounce };
      solids.push(s);
      textBottom = Math.max(textBottom, s.b);
      for (let k = Math.floor(s.t / BAND); k <= Math.floor(s.b / BAND); k++) {
        let a = bands.get(k); if (!a) bands.set(k, (a = [])); a.push(s);
      }
    }
  }
}
new ResizeObserver(resizeBubble).observe(panelArea);
panelContent.addEventListener('scroll', queueGather, { capture: true, passive: true });
new MutationObserver(queueGather).observe(panelContent, { subtree: true, childList: true, characterData: true, attributes: true, attributeFilter: ['class', 'style'] });

function spawn(p) { parts.push({ vx: 0, vy: 0, born: performance.now(), kind: 'board', ...p }); }

/** White glyphs out of a link; push also shoves the glyphs near it out and up. */
function burstFrom(el, { n = 70, v = [2, 5], life = 2600, push = 0 } = {}) {
  if (!el || !B.motion) return;
  const r = el.getBoundingClientRect(), r0 = panelArea.getBoundingClientRect();
  if (!r.width) return; // its tab is closed
  const cx = (r.left + r.right) / 2 - r0.left, cy = (r.top + r.bottom) / 2 - r0.top;
  if (push) {
    for (const p of parts) {
      const dx = p.x - cx, dy = p.y - cy, d = Math.hypot(dx, dy);
      if (d > POP_R || d < 0.1) continue;
      const k = push * (1 - d / POP_R);
      p.vx += (dx / d) * k; p.vy -= (Math.abs(dy) / d) * k * 0.6 + k * 0.4; // out and up, never down
      if (p.line) p.line.broken = true;
      p.word = false; p.post = true;
    }
  }
  for (let i = 0; i < n; i++) {
    const a = (i / n) * Math.PI * 2, sp = rnd(v[0], v[1]);
    spawn({ ch: pickCh(), x: cx, y: cy, vx: Math.cos(a) * sp, vy: Math.sin(a) * sp, kind: 'burst', drag: true, life });
  }
}
const uploadOk = () => burstFrom(document.getElementById('uploadBtn'));
function serialPop() {
  const b = document.getElementById('serialBtn'); if (!b) return;
  b.classList.add('pulse'); setTimeout(() => b.classList.remove('pulse'), 220);
  burstFrom(b, { n: 16, v: [1.2, 2.6], life: 1100, push: 3.2 });
}

function step(now) {
  requestAnimationFrame(step);
  if (!B.motion) return;
  bctx.save();
  bctx.globalCompositeOperation = 'destination-out';
  bctx.fillStyle = 'rgba(0,0,0,0.3)';
  bctx.fillRect(0, 0, BW, BH);
  bctx.restore();
  bctx.textBaseline = 'middle';
  let font = FONT, fill = ''; bctx.font = font;

  // Serial line: one per 150 ms, newest wins; it rises as a whole word.
  if (queued && now - lastLinePrint >= 150) {
    lastLinePrint = now;
    const text = queued; queued = null;
    const line = { glyphs: [], broken: false };
    const x0 = rnd(10, Math.max(12, BW - text.length * CHARW - 10));
    for (let i = 0; i < text.length; i++) {
      if (text[i] === ' ') continue;
      spawn({ ch: text[i], x: x0 + i * CHARW, y: BH - 22, vy: -1.6, line, word: true });
      line.glyphs.push(parts[parts.length - 1]);
    }
  }
  // Floaters: few when idle, more while the agent works.
  const gap = FLOATER_MS[B.state] ?? FLOATER_MS.idle;
  if (now - lastSyn > gap * rnd(0.5, 1.5)) {
    lastSyn = now;
    spawn({ ch: pickCh(), x: rnd(8, BW - 8), y: BH - 6, vy: -0.4, kind: 'syn' });
  }
  // Exhaust from the bottom, centered on the 170-px panel.
  const speed = SPEED[B.state] ?? 0;
  const cx = Math.min(BW, MIN_W) / 2, spread = MIN_W * 0.18;
  exhaustAcc += EXHAUST[B.state] ?? 0;
  while (exhaustAcc >= 1) {
    exhaustAcc -= 1;
    spawn({ ch: pickCh(), x: cx + rnd(-spread, spread), y: BH - 4, vx: rnd(-0.3, 0.3), vy: rnd(-6, -3.5) * speed, kind: 'syn', life: 4000 });
  }

  const rise = speed ? Math.max(RISE, THRUST_RISE * RISE * speed) : RISE;
  const falling = B.state === 'error';
  const alive = [];
  for (const p of parts) {
    const age = now - p.born, life = p.life ?? LIFE;
    if (age > life) continue;
    const whole = p.word && !p.line.broken;
    if (falling) { p.vy += 0.09; p.vx *= 0.98; p.word = false; }
    else if (p.drag) { p.vx *= 0.96; p.vy = p.vy * 0.96 - 0.03; }
    else if (whole) { p.vy = Math.max(-rise, p.vy - 0.01); p.vx = 0; }
    else {
      if (age > 1000) { p.vx += rnd(-0.005, 0.005); p.vy += rnd(-0.01, 0.01); }
      if (p.vy > -rise) p.vy = Math.max(-rise, p.vy - 0.01);
      else if (p.vy < -rise * 1.2) p.vy += 0.04;
      p.vx *= 0.99;
    }
    const mdx = p.x - mouse.x, mdy = p.y - mouse.y, md = Math.hypot(mdx, mdy);
    if (md < 34 && md > 0.1) { p.vx += (mdx / md) * 0.35; p.vy += (mdy / md) * 0.35; p.post = true; } // a pushed glyph collides
    p.x += p.vx; p.y += p.vy;

    let dim = 1; p.flow = false;
    for (const s of bands.get(Math.floor(p.y / BAND)) || []) {
      if (p.x <= s.l || p.x >= s.r || p.y <= s.t || p.y >= s.b) continue;
      const dir = (p.x - s.l) < (s.r - p.x) ? -1 : 1;
      if (!s.bounce) { p.vy *= 0.995; p.flow = true; dim = 0.5; continue; } // flow through green, no push
      if (p.word && !p.line.broken) {
        p.line.broken = true;
        for (const q of p.line.glyphs) { q.word = false; q.vx = rnd(-2.2, 2.2); q.vy = rnd(-1.5, 1); q.post = true; }
      }
      p.x = dir < 0 ? s.l - 0.5 : s.r + 0.5;
      p.vx += dir * rnd(0.4, 1.6); p.vy *= -rnd(0.2, 0.5);
      p.post = true;
    }
    if (p.y < textBottom) p.post = true;
    if (falling && p.y > BH - 6) { p.y = BH - 6; p.vy *= -0.3; }
    if (p.x < -10 || p.x > BW + 10 || p.y < -16 || p.y > BH + 20) continue;

    let a = 0.9 * dim;
    if (life - age < 6000) a *= (life - age) / 6000;
    if (p.kind === 'burst') a = 0.95 * (1 - age / life);
    else if (p.kind === 'syn' && B.state === 'idle') a *= 0.5;
    const want = p.flow ? FONT_FLOW : FONT;
    if (want !== font) { font = want; bctx.font = font; }
    const hex = p.kind === 'burst' ? '#ffffff' : glyphColor(p);
    if (hex !== fill) { fill = hex; bctx.fillStyle = hex; }
    bctx.globalAlpha = Math.max(0, Math.min(1, a));
    bctx.fillText(p.ch, p.x, p.y);
    alive.push(p);
  }
  bctx.globalAlpha = 1;
  parts = alive.length > MAX_GLYPHS ? alive.slice(alive.length - MAX_GLYPHS) : alive;

  // Glyph-to-glyph collisions. A pair is tested when one of the two rebounded, rose above the
  // words or was pushed by the mouse, so the rising column never collides with itself.
  const CELL = CHARW * 2, R = CHARW * 1.5, grid = new Map();
  const cellKey = (x, y) => ((x / CELL) | 0) * 4096 + ((y / CELL) | 0);
  for (const p of parts) {
    if (p.flow) continue;
    const k = cellKey(p.x, p.y);
    let b = grid.get(k); if (!b) grid.set(k, (b = [])); b.push(p);
  }
  for (const p of parts) {
    if (!p.post || p.flow) continue;
    const k0 = cellKey(p.x, p.y);
    for (let dx = -1; dx <= 1; dx++) for (let dy = -1; dy <= 1; dy++) {
      const b = grid.get(k0 + dx * 4096 + dy); if (!b) continue;
      for (const q of b) {
        if (q === p) continue;
        const ex = p.x - q.x, ey = p.y - q.y, d2 = ex * ex + ey * ey;
        if (d2 > 0.01 && d2 < R * R) {
          const d = Math.sqrt(d2), nx = ex / d, ny = ey / d;
          const dv = (p.vx - q.vx) * nx + (p.vy - q.vy) * ny;
          if (dv < 0) { p.vx -= dv * nx * 0.5; p.vy -= dv * ny * 0.5; q.vx += dv * nx * 0.5; q.vy += dv * ny * 0.5; }
          const o = (R - d) * 0.5;
          p.x += nx * o; p.y += ny * o; q.x -= nx * o; q.y -= ny * o;
        }
      }
    }
  }
}
requestAnimationFrame(step);
// While Serial is on, ||Serial|| pops every 2 s.
setInterval(() => { if (B.serial && B.motion) serialPop(); }, POP_MS);

let lastTether = {};
// Help panel: last status, AI Tether off when last drawn, Details open.
let gLast = null;
let gTetherOff = false;
let gOpen = true;
/** White hot links for the running job and Serial; AI Tether turns amber while an agent works. */
function hotButtons() {
  const st = B.state;
  document.getElementById('compileBtn')?.classList.toggle('hot', st === 'compile' || st === 'agentCompile');
  document.getElementById('uploadBtn')?.classList.toggle('hot', st === 'upload' || st === 'agentUpload');
  document.getElementById('serialBtn')?.classList.toggle('hot', B.serial);
  setServer(lastTether);
}
function setBubble(st) {
  B.state = st;
  hotButtons();
}
/** AI Tether's state (polled every 0.5 s); an error shown now stays for its 5 s. */
function bubbleFromServer(st) {
  B.server = st;
  if (!bubbleErrorTimer && st !== B.state) setBubble(st);
}
function bubbleError() {
  clearTimeout(bubbleErrorTimer);
  bubbleErrorTimer = setTimeout(() => { bubbleErrorTimer = null; setBubble(B.server); }, ERROR_MS);
  setBubble('error');
}
function setSerial(on) {
  if (on === B.serial) return;
  B.serial = on;
  if (!on) queued = null;
  hotButtons();
}
let motionSetting = true;
function setMotion(on) {
  motionSetting = on;
  B.motion = on && !matchMedia('(prefers-reduced-motion: reduce)').matches;
  if (!B.motion) { parts = []; queued = null; bctx.clearRect(0, 0, BW, BH); }
  const eye = document.getElementById('eyeBtn');
  if (eye) {
    eye.classList.toggle('off', !on);
    eye.setAttribute('aria-pressed', String(!on));
    eye.title = on ? 'Hide the moving glyphs' : 'Show the moving glyphs';
  }
}
/** Eye click: flip at once, then save to arduinoMcp.motion (which posts back the same value). */
function flipMotion() {
  setMotion(!motionSetting);
  vscode.postMessage({ type: 'setMotion', on: motionSetting });
}
resizeBubble();
setMotion(true);

function toggleSerial() { vscode.postMessage({ type: 'toggleSerial' }); }
function setServer(state) {
  lastTether = state;
  const agent = !!state.serverRunning && AGENT.has(B.state);
  const blocks = document.querySelectorAll('.serverBlock');
  const statuses = document.querySelectorAll('.serverStatus');
  blocks.forEach(block => {
    if (!state.serverRunning) block.style.color='#593b3bff';
    else if (state.serverHealthy) block.style.color='#49c06b';
    else block.style.color='#d2b046';
  });
  statuses.forEach(status => {
    status.classList.toggle('agent-on', agent);
    if (!state.serverRunning) status.textContent='stopped';
    else if (agent) status.textContent='agent working';
    else if (state.serverHealthy) status.textContent='running';
    else status.textContent='starting...';
  });
  if (gLast && gTetherOff === !!state.serverRunning) renderGrease(gLast);
}

let exRows = [];
function renderEx() {
  const q = (document.getElementById('exQuery')?.value || '').trim().toLowerCase();
  const lib = (document.getElementById('exLibrary')?.value || '').trim().toLowerCase();
  const filtered = exRows.filter(r => {
    const full = (r.library + ' ' + r.example + ' ' + r.fullPath).toLowerCase();
    return (!lib || r.library.toLowerCase().includes(lib)) && (!q || full.includes(q));
  });
  const countEl = document.getElementById('exCount');
  const outEl = document.getElementById('exOut');
  if (countEl) countEl.textContent = filtered.length + ' example sketches';
  if (!outEl) return;
  if (!filtered.length) { outEl.innerHTML = '<div class="muted" style="font-size:10px">No examples found.</div>'; return; }
  outEl.innerHTML = filtered.map(r =>
    '<div class="mgr-item" style="cursor:pointer" data-path="' + esc(r.fullPath) + '">' +
    '<div class="name">' + esc(r.example) + '</div>' +
    '<div class="meta">' + esc(r.library) + '</div></div>'
  ).join('');
  outEl.querySelectorAll('.mgr-item').forEach(el => {
    el.addEventListener('click', () => vscode.postMessage({type:'openExample', path: el.getAttribute('data-path') || ''}));
  });
}

window.addEventListener('message', event => {
  const msg = event.data;
  if (msg.type === 'state') {
    const s = msg.state || {};
    document.getElementById('portVal').textContent = s.port || '?';
    const fqbnVal = s.fqbn || '?';
    document.getElementById('fqbnVal').textContent = fqbnVal;
    if (fqbnVal === '?') {
      document.getElementById('fqbnVal').innerHTML += '<br><span style="font-size:8px;color:#ffb4b4;line-height:1.2">Unknown board. Access ||Managers||.</span>';
    }
    setServer(s);
    const sn = document.getElementById('setupNeeded');
    if (sn) sn.style.display = s.setupNeeded ? 'block' : 'none';
    setSerial(!!s.serialActive);
    if (s.accentColor) {
      const block = document.getElementById('accentBlock');
      if (block) block.style.color = s.accentColor;
      const idx = ACCENT_COLORS.indexOf(s.accentColor);
      if (idx !== -1) accentIndex = idx;
    }
  } else if (msg.type === 'accentColor') {
    const block = document.getElementById('accentBlock');
    if (block) block.style.color = msg.color;
    const idx = ACCENT_COLORS.indexOf(msg.color);
    if (idx !== -1) accentIndex = idx;
  } else if (msg.type === 'verifyResult') {
    if (!msg.success) bubbleError();
  } else if (msg.type === 'examples') {
    exRows = Array.isArray(msg.rows) ? msg.rows : [];
    const errEl = document.getElementById('exError');
    if (errEl) errEl.textContent = '';
    renderEx();
  } else if (msg.type === 'exError') {
    const errEl = document.getElementById('exError');
    if (errEl) errEl.textContent = msg.error || 'Unknown error';
  } else if (msg.type === 'uploadResult') {
    if (msg.success) uploadOk(); else bubbleError();
  } else if (msg.type === 'bubble') {
    bubbleFromServer(String(msg.state || 'idle'));
  } else if (msg.type === 'serialLine') {
    if (B.serial && B.motion) queued = String(msg.line || '').slice(0, 48);
  } else if (msg.type === 'motion') {
    setMotion(msg.on !== false);
  } else if (msg.type === 'boards') {
    boards = Array.isArray(msg.rows) ? msg.rows : [];
    if (!selectedFqbn && boards.length) selectedFqbn = boards[0].fqbn;
    if (typeof msg.query === "string") boardQuery = msg.query;
    boardMode = 'installed';
    selectedCatalogId = null;
    const iBtn = document.getElementById('installBoardBtn');
    if (iBtn) iBtn.textContent = '||Install new board \u2197||';
    renderBoards();
  } else if (msg.type === 'libraries') {
    libs = Array.isArray(msg.rows) ? msg.rows : [];
    if (!selectedLib && libs.length) selectedLib = libs[0].name;
    renderLibs();
  } else if (msg.type === 'mgrError') {
    const el = document.getElementById('mgrErr');
    // Double backslash below: this script sits in a TS template literal, where a
    // single one turns into a real line break and the whole script stops parsing.
    const lines = String(msg.error || '').split('\\n').map(l => l.trim()).filter(Boolean);
    const explained = lines.filter(l => /^(Why|What):/.test(l));
    if (el) el.textContent = (explained.length ? explained : lines.slice(-2)).join('\\n');
    const iBtn = document.getElementById('installBoardBtn');
    if (iBtn && iBtn.textContent === '||Installing...||') iBtn.textContent = '||Install new board \u2197||';
  } else if (msg.type === 'greaseStatus') {
    renderGrease(msg.status || {});
  } else if (msg.type === 'greaseError') {
    const el = document.getElementById('gErr');
    if (el) el.textContent = String(msg.error || '');
  } else if (msg.type === 'switchTab') {
    switchTab(msg.tab === 'grease' ? 'grease' : 'board');
  } else if (msg.type === 'gitInstalled') {
    const g = document.getElementById('libGitInput');
    if (g) g.value = '';
  }
});

let boards = [];
let libs = [];
let selectedFqbn = "";
let selectedLib = "";
let boardQuery = "";
let boardMode = 'installed';
let selectedCatalogId = null;
const BOARD_CATALOG = ${catalogJson};
const INITIAL_TAB = ${initialTab};

function esc(s){return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;').replace(/'/g,'&#39;');}

function renderBoards() {
  if (boardMode === 'catalog') { renderCatalogBoards(); return; }
  const filtered = boards.filter((b) => {
    const q = boardQuery.toLowerCase();
    if (!q) return true;
    return (b.name + " " + b.fqbn + " " + b.platform).toLowerCase().includes(q);
  });
  if (!filtered.length) {
    document.getElementById('boardsList').innerHTML = "<div class='muted' style='font-size:10px'>No installed boards found.</div>";
    return;
  }
  document.getElementById('boardsList').innerHTML = filtered.map((b) => {
    const border = selectedFqbn === b.fqbn ? "border-color: #58aa58; box-shadow: 0 0 0 1px #58aa58 inset;" : "";
    return "<div class='mgr-item' style='" + border + "' data-fqbn='" + esc(b.fqbn) + "'><div class='name'>" + esc(b.name) + "</div><div class='meta'>" + esc(b.fqbn) + " | " + esc(b.platform) + " @ " + esc(b.version) + "</div></div>";
  }).join("");
  document.querySelectorAll("#boardsList .mgr-item").forEach((el) => {
    el.addEventListener("click", () => {
      selectedFqbn = el.getAttribute("data-fqbn") || "";
      renderBoards();
    });
  });
}

function renderCatalogBoards() {
  const q = boardQuery.toLowerCase();
  const filtered = q ? BOARD_CATALOG.filter(b =>
    (b.name + ' ' + b.vendor + ' ' + b.id + ' ' + b.boards.join(' ')).toLowerCase().includes(q)
  ) : BOARD_CATALOG;
  if (!filtered.length) {
    document.getElementById('boardsList').innerHTML = "<div class='muted' style='font-size:10px'>No matching boards.</div>";
    return;
  }
  document.getElementById('boardsList').innerHTML = filtered.map((b) => {
    const border = selectedCatalogId === b.id ? "border-color: #f6c542; box-shadow: 0 0 0 1px #f6c542 inset;" : "";
    return "<div class='mgr-item' style='" + border + "' data-catalog-id='" + esc(b.id) + "'><div class='name'>" + esc(b.name) + "</div><div class='meta'>" + esc(b.vendor) + " | " + esc(b.boards.slice(0,3).join(', ')) + "</div></div>";
  }).join("");
  document.querySelectorAll("#boardsList [data-catalog-id]").forEach((el) => {
    el.addEventListener("click", () => {
      selectedCatalogId = el.getAttribute("data-catalog-id") || "";
      renderCatalogBoards();
    });
    el.addEventListener("dblclick", () => {
      selectedCatalogId = el.getAttribute("data-catalog-id") || "";
      installSelectedCatalogBoard();
    });
  });
}

function clearMgrErr() { const el = document.getElementById('mgrErr'); if (el) el.textContent = ''; }

function installSelectedCatalogBoard() {
  if (!selectedCatalogId) return;
  clearMgrErr();
  const entry = BOARD_CATALOG.find(b => b.id === selectedCatalogId);
  if (!entry) return;
  const btn = document.getElementById('installBoardBtn');
  if (btn) btn.textContent = '||Installing...||';
  vscode.postMessage({ type: 'boardCatalogInstall', id: entry.id });
}

function renderLibs() {
  if (!libs.length) {
    document.getElementById('libsList').innerHTML = "<div class='muted' style='font-size:10px'>No libraries loaded.</div>";
    return;
  }
  document.getElementById('libsList').innerHTML = libs.map((l) => {
    const right = l.version || l.author || l.sentence || "";
    const border = selectedLib === l.name ? "border-color: #58aa58; box-shadow: 0 0 0 1px #58aa58 inset;" : "";
    return "<div class='mgr-item' style='" + border + "' data-lib='" + esc(l.name) + "'><div class='name'>" + esc(l.name) + "</div><div class='meta'>" + esc(right) + "</div></div>";
  }).join("");

  document.querySelectorAll("#libsList .mgr-item").forEach((el) => {
    el.addEventListener("click", () => {
      selectedLib = el.getAttribute("data-lib") || "";
      renderLibs();
    });
  });
}

const $ = (id) => document.getElementById(id);
$("updateIdx")?.addEventListener("click", () => {
  boardMode = 'installed';
  selectedCatalogId = null;
  const btn = $("installBoardBtn");
  if (btn) btn.textContent = '||Install new board \u2197||';
  vscode.postMessage({ type: "updateIndexes" });
});
$("boardQuery")?.addEventListener("keydown", (e) => {
  if(e.key === 'Enter') {
    boardQuery = $("boardQuery").value || "";
    if (boardMode === 'catalog') { renderCatalogBoards(); }
    else { vscode.postMessage({ type: "boardSearch", query: boardQuery }); }
  }
});
$("boardQuery")?.addEventListener("input", () => {
  if (boardMode === 'catalog') { boardQuery = $("boardQuery").value || ""; renderCatalogBoards(); }
});
$("installBoardBtn")?.addEventListener("click", () => {
  if (boardMode === 'installed') {
    boardMode = 'catalog';
    selectedCatalogId = null;
    boardQuery = $("boardQuery")?.value || "";
    const btn = $("installBoardBtn");
    if (btn) btn.textContent = '||Confirm install||';
    renderCatalogBoards();
  } else {
    installSelectedCatalogBoard();
  }
});
$("chooseTargetBtn")?.addEventListener("click", () => vscode.postMessage({ type: "chooseTarget", fqbn: selectedFqbn }));
$("recoveryUploadBtn")?.addEventListener("click", () => vscode.postMessage({ type: "recoveryUpload" }));
$("libListBtn")?.addEventListener("click", () => vscode.postMessage({ type: "libList" }));
$("libQuery")?.addEventListener("keydown", (e) => {
  if(e.key === 'Enter') {
    vscode.postMessage({ type: "libSearch", query: $("libQuery").value });
  }
});
$("libInstallBtn")?.addEventListener("click", () => { clearMgrErr(); vscode.postMessage({ type: "libInstallSelected", name: selectedLib }); });

// ── Help panel ────────────────────────────────────────────────────────
function greaseRefresh(checks) {
  const el = document.getElementById('gErr');
  if (el) el.textContent = '';
  vscode.postMessage({ type: 'greaseRefresh', checks: !!checks });
}
function gCode(text) {
  return '<span class="g-code">' + esc(text) + '</span><a class="c-std" href="#" data-copy="' + esc(text) + '">||Copy||</a>';
}
function gSkill(st) {
  if (st === 'current') return '<span class="g-ok">&#10003;</span>';
  if (st === 'outdated') return '<span class="g-warn">old</span>';
  if (st === 'edited') return '<span class="g-warn">edited by you</span>';
  if (st === 'foreign') return '<span class="g-warn">another grease skill</span>';
  if (st === 'n/a') return '<span class="dim">&ndash;</span>';
  return '<span class="g-bad">&#10007;</span>';
}
function gTools(r) {
  if (r.tools === 'added') return '<span class="g-ok">&#10003;</span>';
  if (r.tools === 'built-in') return '<span class="g-ok">&#10003; built in</span>';
  if (r.tools === 'unreadable') return '<span class="g-bad">config unreadable</span>';
  return '<a class="c-green" href="#" data-add="' + esc(r.id) + '">||' + (r.tools === 'old' ? 'Replace old entry' : 'Add tools') + '||</a>';
}
function gLine(mark, cls, text, right, tip) {
  return '<div class="g-row g-line"' + (tip ? ' title="' + esc(tip) + '"' : '') + '><span><span class="' + cls + '">' + mark + '</span> ' + text + '</span><span>' + (right || '') + '</span></div>';
}
const gSkillOk = (r) => ['current', 'edited', 'n/a'].includes(r.skill);
const gToolsOk = (r) => ['added', 'built-in', 'n/a'].includes(r.tools);
function gAgentLine(r) {
  const tip = 'skill: ' + r.skill + ', tools: ' + r.tools;
  if (gSkillOk(r) && gToolsOk(r)) return gLine('&#10003;', 'g-ok', esc(r.name), '<span class="dim">' + esc(r.invoke) + '</span>', tip);
  let right = gTools(r);
  if (gToolsOk(r)) right = r.skill === 'foreign' ? gSkill(r.skill) : '<a class="c-green" href="#" data-act="gReinstall">||Reinstall skill||</a>';
  return gLine('&#9675;', 'g-warn', esc(r.name), right, tip);
}
function renderGrease(s) {
  gLast = s;
  const port = document.getElementById('gPort');
  if (port) port.textContent = s.tetherPort ? ':' + s.tetherPort : '';
  const c = s.cli;
  const n = s.node;
  const j = s.job;
  const busy = j && !j.failed;
  // Problems go on top, above Details.
  let fix = '';
  let count = 0;
  let setup = '';
  let installing = false;
  if (c && c.state === 'ok') {
    setup += gLine('&#10003;', 'g-ok', 'arduino-cli ' + esc(c.version || ''), '', c.path);
  } else if (c && c.state === 'offpath') {
    setup += gLine('&#9888;', 'g-warn', 'arduino-cli ' + esc(c.version || '') + ', not on PATH', '', c.path);
    setup += '<div class="g-note">Grease uses this copy. For terminals and other agents, run once:</div>' + gCode(c.fix);
  } else if (c && !busy) {
    const line = gLine('&#10007;', 'g-bad', 'arduino-cli', '<a class="c-amber" href="#" data-act="gPrepareCli">||Install &#8599;||</a>');
    setup += line;
    fix += line;
    count++;
    installing = true;
    if (j && j.kind === 'cli' && j.failed) fix += '<div class="g-note">Last try: ' + esc(j.failed) + '</div>';
    fix += '<div class="g-note">Or install it yourself:</div>' + gCode(c.install);
    (c.copies || []).forEach((p) => {
      fix += '<div class="g-row"><span class="g-path">' + esc(p) + '</span><a class="c-amber" href="#" data-usecli="' + esc(p) + '">||Use this copy||</a></div>';
    });
  }
  if (c && c.state !== 'missing' && !busy) {
    const retry = '<a class="c-amber" href="#" data-act="gPrepareCli">||Retry &#8599;||</a>';
    if (j && j.kind === 'firstRun' && j.failed) { fix += gLine('&#10007;', 'g-bad', 'first run: ' + esc(j.failed), retry); count++; }
    else if (c.firstRun && c.firstRun.length) { fix += gLine('&#9888;', 'g-warn', 'first run not finished: ' + esc(c.firstRun.join(', ')), retry); count++; }
  }
  if (n && n.state === 'ok') {
    setup += gLine('&#10003;', 'g-ok', 'Node.js ' + esc(n.version || ''), '', n.path);
  } else if (n && n.state === 'editor') {
    setup += gLine('&#9888;', 'g-warn', 'Node.js not found') + '<div class="g-note">Outside agents use this editor&#39;s Node. Install Node.js so they keep working:</div>' + gCode(n.install);
  } else if (n) {
    const line = gLine('&#10007;', 'g-bad', 'Node.js');
    setup += line;
    fix += line + '<div class="g-note">Needed for agents outside the editor:</div>' + gCode(n.install);
    count++;
    installing = true;
  }
  (s.linux || []).forEach((w) => {
    const row = gLine(w.blocking ? '&#10007;' : '&#9888;', w.blocking ? 'g-bad' : 'g-warn', esc(w.title), '', '')
      + '<div class="g-note">' + esc(w.why) + '</div>'
      + (w.fix ? gCode(w.fix) : '')
      + (w.note ? '<div class="g-note">' + esc(w.note) + '</div>' : '');
    setup += row;
    if (w.blocking) { fix += row; count++; }
  });
  if (installing) fix += '<div class="g-note">Restart the editor after installing.</div>';
  gTetherOff = lastTether.serverRunning === false;
  if (gTetherOff) {
    fix += gLine('&#10007;', 'g-bad', 'AI Tether off', '<a class="c-amber" href="#" data-tether>||Turn on||</a>');
    count++;
  }

  const rows = Array.isArray(s.agents) ? s.agents : [];
  const found = rows.filter((r) => r.present);
  const absent = rows.filter((r) => !r.present);
  const absentLinks = absent.map((r) => '<a class="dim" href="#" data-open="' + esc(r.id) + '">' + esc(r.name) + '</a>').join(', ');
  found.filter((r) => !gSkillOk(r) || !gToolsOk(r)).forEach((r) => {
    fix += gAgentLine(r);
    count++;
  });
  if (rows.length && !found.length) {
    fix += '<div class="g-note">No agent found. Install one: ' + absentLinks + '</div>';
    count++;
  }

  let verdict;
  if (busy) {
    const f = typeof j.fraction === 'number' ? Math.max(0, Math.min(1, j.fraction)) : null;
    const bar = f === null ? '~~~~~~~~~~' : '#'.repeat(Math.round(f * 10)) + '.'.repeat(10 - Math.round(f * 10));
    verdict = '<div class="g-row"><span class="g-warn" style="white-space:pre">[' + bar + ']</span><span>' + (f === null ? '' : Math.round(f * 100) + '% ') + esc(j.label) + '</span></div>';
  } else if (!c || !n) {
    verdict = '<span class="dim">checking...</span>';
  } else if (count) {
    verdict = '<span class="g-bad">&#10007; ' + count + ' to fix</span>';
  } else {
    verdict = '<span class="g-ok">&#10003; Ready</span><div class="g-hint">Type <b>/grease</b>, then ask.</div>';
  }
  document.getElementById('gVerdict').innerHTML = verdict;
  document.getElementById('gFix').innerHTML = fix;
  document.getElementById('gSetup').innerHTML = setup;
  let a = found.map(gAgentLine).join('');
  if (found.length && absent.length) a += '<div class="g-note">Not found: ' + absentLinks + '</div>';
  document.getElementById('gAgents').innerHTML = a;
}
function gToggleDetails() {
  gOpen = !gOpen;
  document.getElementById('gDetails').hidden = !gOpen;
  document.getElementById('gToggle').innerHTML = (gOpen ? '&#9662;' : '&#9656;') + ' Details';
}
document.getElementById('panel-grease')?.addEventListener('click', (e) => {
  const t = e.target.closest('[data-copy],[data-usecli],[data-act],[data-add],[data-open],[data-toggle],[data-tether]');
  if (!t) return;
  e.preventDefault();
  if (t.hasAttribute('data-toggle')) gToggleDetails();
  else if (t.hasAttribute('data-tether')) cmd('arduinoMcp.toggleServer');
  else if (t.hasAttribute('data-copy')) vscode.postMessage({ type: 'gCopy', text: t.getAttribute('data-copy') });
  else if (t.hasAttribute('data-usecli')) vscode.postMessage({ type: 'gUseCli', path: t.getAttribute('data-usecli') });
  else if (t.hasAttribute('data-act')) vscode.postMessage({ type: t.getAttribute('data-act') });
  else if (t.hasAttribute('data-add')) { t.textContent = '||Adding...||'; vscode.postMessage({ type: 'gAddTools', id: t.getAttribute('data-add') }); }
  else if (t.hasAttribute('data-open')) vscode.postMessage({ type: 'gOpen', id: t.getAttribute('data-open') });
});
if (INITIAL_TAB !== 'board') switchTab(INITIAL_TAB);

function onUploadClick(){cmd('arduinoMcp.upload');}
    </script>
  </body>
</html>`;
  }
}
