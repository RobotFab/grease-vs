/**
 * ui/plotterWebview.mjs → dist/plotter.js
 * ──────────────────────────────────────
 * The Plotter inside its VS Code webview. Rows come from
 * serialPlotterPanel.ts, which reads AI Tether's plot buffer; the view is
 * plotterView.mjs, the same one the MCP App uses.
 *
 * Mode, backgrounds and hidden series survive the panel being hidden.
 */

import { mountPlotter } from "./plotterView.mjs";

const vscode = acquireVsCodeApi();
const $ = (id) => document.getElementById(id);

const view = mountPlotter({ saved: vscode.getState() ?? {}, save: (s) => vscode.setState(s) });

function setStatus(s) {
  if (!s) return;
  const open = !!s.isOpen;
  $("status").textContent = open ? ` ${s.path} @ ${s.baudRate}` : "Disconnected";
  $("toggle").textContent = open ? "||S|| █running " : "||S|| █stopped ";
  $("toggle").style.color = open ? "#49c06b" : "#593b3b";
}

$("toggle").addEventListener("click", () => {
  vscode.postMessage({ type: "toggle", path: $("port").value, baudRate: Number($("baud").value || 9600) });
});
$("clear").addEventListener("click", () => vscode.postMessage({ type: "clear" }));

window.addEventListener("message", (event) => {
  const msg = event.data;
  if (msg.type === "plot") view.plot(msg);
  else if (msg.type === "status") setStatus(msg.status);
});
