/**
 * ui/plotterApp.mjs → dist/plotter-app.html (ui://grease/plotter)
 * ──────────────────────────────────────────────────────────────
 * The Plotter as an MCP App (ADR 0009), for hosts that show MCP Apps
 * (Claude Desktop, VS Code Copilot). openPlotter shows it in the chat; it
 * polls the app-only tool plotRows for the rows after the last seq it saw,
 * so it takes no lines from serialRead. The view is plotterView.mjs, the
 * same one the VS Code panel uses.
 *
 * AI Tether also serves this page at /plotter?v=<key>, for hosts that draw no
 * MCP Apps. With that key the page polls /plotter/rows instead of plotRows.
 *
 * The agent opens and closes the serial port; the app only shows it.
 */

import { App } from "@modelcontextprotocol/ext-apps/app-with-deps";
import { mountBubble } from "./plotterBubble.mjs";
import { mountPlotter } from "./plotterView.mjs";

const POLL_MS = 250;
const STATE_KEY = "grease-plotter";
const BUBBLE_KEY = "grease-plotter-bubble";
const $ = (id) => document.getElementById(id);
const pageParams = new URLSearchParams(location.search);
const pageKey = pageParams.get("v");

// The host's frame may refuse storage; the view then starts fresh each time.
let saved = {};
try { saved = JSON.parse(localStorage.getItem(STATE_KEY) ?? "{}"); } catch {}
const view = mountPlotter({
  saved,
  save: (s) => { try { localStorage.setItem(STATE_KEY, JSON.stringify(s)); } catch {} },
  transparent: true,
});

let bubbleOn = true;
try { bubbleOn = localStorage.getItem(BUBBLE_KEY) !== "off"; } catch {}
const bubble = mountBubble($("bubble"), { enabled: bubbleOn });

const app = pageKey ? null : new App({ name: "Grease Plotter", version: GREASE_VERSION });
let seq = 0;
let alive = true;

function setStatus(s) {
  const open = !!s?.isOpen;
  $("status").textContent = !s ? "AI Tether not answering" : open ? `${s.path} @ ${s.baudRate}` : "No serial session: ask the agent to open one";
  $("run").textContent = open ? "█running" : "█stopped";
  $("run").style.color = open ? "#49c06b" : "#593b3b";
}

async function fetchRows(clear) {
  if (!app) {
    const r = await fetch(`/plotter/rows?v=${encodeURIComponent(pageKey)}`, {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify({ since: seq, clear }),
    });
    if (!r.ok) throw new Error(`Plotter rows: HTTP ${r.status}`);
    return r.json();
  }
  const r = await app.callServerTool({ name: "plotRows", arguments: { since: seq, ...(clear ? { clear: true } : {}) } });
  return r.structuredContent;
}

async function poll(clear = false) {
  const p = await fetchRows(clear);
  if (!p) throw new Error("plotRows returned no rows");
  if (p.cleared || p.rows.length || seq === 0) view.plot({ ...p, cleared: p.cleared || seq === 0 });
  seq = p.seq;
  bubble.setState(p.bubble ?? "idle");
  setStatus(p.serial);
}

let busy = false;
async function tick(clear = false) {
  if (busy || !alive || (document.hidden && !clear)) return;
  busy = true;
  try {
    await poll(clear);
  } catch {
    setStatus(null);
  } finally {
    busy = false;
  }
}

$("clear").addEventListener("click", () => void tick(true));
$("motion").addEventListener("click", () => {
  bubbleOn = !bubbleOn;
  bubble.setEnabled(bubbleOn);
  try { localStorage.setItem(BUBBLE_KEY, bubbleOn ? "on" : "off"); } catch {}
});

// openPlotter's arguments: the mode to show ("auto" follows the Plot Pairs).
if (app) {
  app.ontoolinput = (params) => view.setMode(params?.arguments?.mode ?? "auto");
  app.onteardown = () => {
    alive = false;
    return {};
  };
  await app.connect();
} else {
  document.body.classList.add("page");
  view.setMode(pageParams.get("mode") ?? "auto");
}
setInterval(() => void tick(), POLL_MS);
void tick();
