/**
 * ui/serialPlotterPanel.ts
 * ────────────────────────
 * The Plotter in a VS Code webview panel (ADR 0009): Time, XY and Radar tabs.
 *
 * AI Tether parses each serial line into its plot buffer; this panel polls
 * POST /plot for the rows after the last seq it saw and posts them to the
 * webview. Reading the plot buffer takes no lines from the Serial Monitor
 * or an agent's serialRead. Drawing lives in dist/plotter.js
 * (src/ui/plotterWebview.mjs + server/lib/plotDraw.mjs).
 */

import * as vscode from "vscode";
import * as os from "node:os";
import { randomBytes } from "node:crypto";
import { plotRead, serialClose, serialOpen } from "../serverHttpClient";

export class SerialPlotterPanel {
  private panel: vscode.WebviewPanel | null = null;
  private pollTimer: NodeJS.Timeout | null = null;
  /** This panel opened the port (so closing the panel closes it). */
  private isConnected = false;
  /** The port is open, by anyone: the panel, the Serial Monitor or an agent. */
  private portOpen = false;
  /** Port to open after the first poll, when no session is open. */
  private pendingPort: string | null = null;
  /** Last plot-buffer seq handed to the webview; 0 = send everything held. */
  private seq = 0;

  constructor(
    private output: vscode.OutputChannel,
    private extensionUri: vscode.Uri,
  ) {}

  /** Reveal (creating if needed) the plotter webview and optionally auto-connect. */
  show(defaultPort: string | null | undefined, autoConnect = true): void {
    if (!this.panel) {
      this.panel = vscode.window.createWebviewPanel(
        "arduinoMcp.serialPlotter",
        "Grease: Plttr",
        vscode.ViewColumn.Beside,
        { enableScripts: true, localResourceRoots: [vscode.Uri.joinPath(this.extensionUri, "dist")] },
      );
      this.panel.onDidDispose(() => this.dispose());
      // A hidden webview is unloaded; when it shows again, send it every row held.
      this.panel.onDidChangeViewState((e) => {
        if (e.webviewPanel.visible) this.seq = 0;
      });
      this.panel.webview.onDidReceiveMessage((msg) => void this.onMessage(msg));
    }
    this.panel.title = "Grease: Plttr";
    this.panel.reveal(vscode.ViewColumn.Beside);
    this.seq = 0;
    this.panel.webview.html = this.html(this.panel.webview, defaultPort);
    this.startPolling();
    this.postStatus();
    // The first poll connects, unless a session (an agent's, say) is open:
    // that one is plotted as it is, since reopening would reset the board.
    this.pendingPort = autoConnect && defaultPort && !this.isConnected ? defaultPort : null;
  }

  private dispose(): void {
    this.panel = null;
    if (this.pollTimer) clearInterval(this.pollTimer);
    this.pollTimer = null;
    if (this.isConnected) {
      void serialClose();
      this.isConnected = false;
    }
  }

  /** Open the serial port via the MCP server, retrying on transient errors (e.g. after board reset). */
  private async connect(port: string, baudRate: number): Promise<void> {
    const MAX_ATTEMPTS = 3;
    const RETRY_DELAY_MS = 600;
    let lastMsg = "";
    for (let attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
      try {
        await serialOpen({ path: port, baudRate });
        this.output.appendLine(`Serial connected (plotter): ${port} @ ${baudRate}`);
        this.isConnected = true;
        this.postStatus();
        return;
      } catch (e) {
        lastMsg = e instanceof Error ? e.message : String(e);
        const isPermDenied =
          lastMsg.includes("Permission denied") || lastMsg.includes("EACCES") || lastMsg.includes("EPERM");
        if (isPermDenied) break; // permission errors won't resolve with retries
        if (attempt < MAX_ATTEMPTS) {
          this.output.appendLine(`Serial open failed (plotter, attempt ${attempt}/${MAX_ATTEMPTS}): ${lastMsg} — retrying in ${RETRY_DELAY_MS}ms`);
          await new Promise((r) => setTimeout(r, RETRY_DELAY_MS));
        }
      }
    }
    const isPermDenied =
      lastMsg.includes("Permission denied") || lastMsg.includes("EACCES") || lastMsg.includes("EPERM");
    if (isPermDenied && os.platform() === "linux") {
      vscode.window
        .showErrorMessage(
          `Serial: Permission denied on ${port}. On Linux, run: sudo usermod -a -G dialout $USER — then log out and back in.`,
          "Copy Command",
        )
        .then((action) => {
          if (action === "Copy Command") {
            vscode.env.clipboard.writeText(`sudo usermod -a -G dialout $USER`);
          }
        });
    } else {
      vscode.window.showErrorMessage(`Serial: Failed to open ${port}: ${lastMsg}`);
    }
    this.output.appendLine(`Serial open failed (plotter): ${lastMsg}`);
  }

  /** Every 150 ms: the plot buffer's new rows and the serial status go to the webview. */
  private startPolling(): void {
    if (this.pollTimer) return;
    let busy = false;
    this.pollTimer = setInterval(() => {
      if (!this.panel || busy) return;
      busy = true;
      void (async () => {
        try {
          const r = await plotRead(this.seq);
          if (r.cleared || r.rows.length || this.seq === 0) {
            this.panel?.webview.postMessage({ type: "plot", rows: r.rows, labels: r.labels, cleared: r.cleared || this.seq === 0 });
          }
          this.seq = r.seq;
          this.portOpen = r.serial.isOpen;
          if (!r.serial.isOpen) this.isConnected = false;
          const port = this.pendingPort;
          this.pendingPort = null;
          if (port && !r.serial.isOpen) await this.connect(port, 9600);
          this.panel?.webview.postMessage({ type: "status", status: r.serial });
        } catch {
          this.panel?.webview.postMessage({ type: "status", status: { isOpen: false } });
        } finally {
          busy = false;
        }
      })();
    }, 150);
  }

  private postStatus(): void {
    if (!this.panel) return;
    this.panel.webview.postMessage({ type: "status", status: { isOpen: this.isConnected } });
  }

  private async onMessage(msg: any): Promise<void> {
    if (!msg?.type) return;
    try {
      if (msg.type === "toggle") {
        const port = String(msg.path || "");
        const baud = Number(msg.baudRate || 9600);
        if (this.isConnected || this.portOpen) {
          await serialClose();
          this.output.appendLine("Serial disconnected (plotter)");
          this.isConnected = false;
        } else {
          await this.connect(port, baud);
        }
        this.postStatus();
      } else if (msg.type === "clear") {
        // Clears AI Tether's plot buffer: readPlot and plotSnapshot start over too.
        await plotRead(this.seq, true);
      }
    } catch (e) {
      const err = e instanceof Error ? e.message : String(e);
      this.output.appendLine(`Serial plotter error: ${err}`);
      vscode.window.showErrorMessage(`Grease Plotter: ${err}`);
      this.postStatus();
    }
  }

  /** Look A: today's green Plotter plus a Time | XY | Radar tab row. */
  private html(webview: vscode.Webview, defaultPort: string | null | undefined): string {
    const portValue = defaultPort ? defaultPort.replaceAll("&", "&amp;").replaceAll('"', "&quot;").replaceAll("<", "&lt;") : "";
    const placeholder = os.platform() === "win32" ? "COM1" : os.platform() === "darwin" ? "/dev/cu.usbmodem..." : "/dev/ttyUSB0";
    const script = webview.asWebviewUri(vscode.Uri.joinPath(this.extensionUri, "dist", "plotter.js"));
    const nonce = randomBytes(16).toString("hex");
    const csp = [
      "default-src 'none'",
      `style-src ${webview.cspSource} 'unsafe-inline'`,
      `script-src 'nonce-${nonce}'`,
    ].join("; ");
    return `<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta http-equiv="Content-Security-Policy" content="${csp}">
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>
      :root { --panel: #0d1611; --ink: #b5ffc8; --muted: #76bf8b; --stroke: #2d4d3a; --mono: Consolas, Menlo, Monaco, "Courier New", monospace; }
      * { box-sizing: border-box; }
      html, body { height: 100%; }
      body { margin: 0; padding: 12px; display: flex; flex-direction: column; background: radial-gradient(circle at 20% 0%, #0c120f, #050806 65%); color: var(--ink); font-family: var(--mono); font-size: 11px; }
      .card { border: 1px solid var(--stroke); background: var(--panel); padding: 8px; border-radius: 8px; margin-bottom: 8px; }
      .row { display: flex; gap: 8px; align-items: flex-end; flex-wrap: wrap; }
      label { font-size: 10px; color: var(--muted); }
      input { background: #07100b; color: var(--ink); border: 1px solid var(--stroke); border-radius: 6px; padding: 6px 7px; font: 11px var(--mono); min-width: 180px; }
      #baud { min-width: 90px; width: 90px; }
      .status { font-size: 10px; color: var(--muted); margin-left: auto; padding-bottom: 6px; }
      .cmdbtn { color: #d8ffd8; cursor: pointer; position: relative; padding-bottom: 6px; user-select: none; }
      .cmdbtn::after { content: attr(data-tip); position: absolute; left: 0; bottom: 120%; background: #0a110a; color: #e5ffe5; border: 1px solid #335233; border-radius: 4px; padding: 2px 4px; font-size: 10px; opacity: 0; pointer-events: none; transition: opacity .12s ease; white-space: nowrap; }
      .cmdbtn:hover::after { opacity: 1; }
      #tabs { display: flex; gap: 2px; }
      #tabs button { background: #07100b; color: #4c7a5a; border: 1px solid var(--stroke); border-bottom: none; border-radius: 6px 6px 0 0; padding: 5px 14px; font: 11px var(--mono); cursor: pointer; }
      #tabs button.lit { color: var(--ink); }
      #tabs button.lit::before { content: "\\25CF  "; color: #7dff9e; }
      #tabs button.on { background: var(--panel); color: #e5ffe5; border-color: #49c06b; }
      canvas { flex: 1; min-height: 160px; width: 100%; display: block; border: 1px solid var(--stroke); border-radius: 0 8px 8px 8px; }
      #legend { display: flex; gap: 10px; flex-wrap: wrap; align-items: center; padding: 5px 8px; margin-top: 4px; border: 1px solid var(--stroke); border-radius: 6px; background: var(--panel); min-height: 26px; font-size: 10px; }
      .leg-item { cursor: pointer; user-select: none; }
      .leg-item:hover { text-decoration: underline; }
      .leg-item.off { opacity: .3; }
      .leg-bg { cursor: pointer; color: var(--muted); border: 1px solid var(--stroke); padding: 1px 6px; border-radius: 3px; margin-left: auto; }
      .leg-bg:hover { color: var(--ink); }
    </style>
  </head>
  <body>
    <div class="card">
      <div class="row">
        <div><label>Port</label><br/><input id="port" placeholder="${placeholder}" value="${portValue}"/></div>
        <div><label>Baud</label><br/><input id="baud" value="9600"/></div>
        <span class="cmdbtn" data-tip="start/stop" id="toggle">||S||</span>
        <span class="cmdbtn" data-tip="clear the Plotter (agents too)" id="clear">||C||</span>
        <div class="status" id="status"></div>
      </div>
    </div>
    <div id="tabs"><button data-m="time">Time</button><button data-m="xy" title="name_x + name_y on one line">XY</button><button data-m="radar" title="name_deg + name_cm on one line">Radar</button></div>
    <canvas id="canvas"></canvas>
    <div id="legend"></div>
    <script nonce="${nonce}" src="${script}"></script>
  </body>
</html>`;
  }
}
