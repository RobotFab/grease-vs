/**
 * ui/plotterView.mjs
 * ──────────────────
 * The Plotter's view (look A: today's green Plotter plus a Time | XY | Radar
 * tab row), shared by the VS Code webview (plotterWebview.mjs) and the MCP
 * App (plotterApp.mjs). Each surface brings its own rows and its own place
 * to keep the view state; drawing is server/lib/plotDraw.mjs, the same code
 * plotSnapshot uses.
 *
 * Tabs are switched by hand; a tab gets a ● once its Plot Pairs appear.
 */

import { FADE_MS, plotPairs } from "../../server/lib/plot.mjs";
import { bestMode, colorOf, drawPlot, MODES, pairOf, THEMES } from "../../server/lib/plotDraw.mjs";

const WIN_MS = 20000;
const KEEP_MS = 60000;
const MAX_ROWS = 6000;

/**
 * Mount the Plotter on #canvas, #tabs and #legend.
 * `saved` is the view state kept earlier; `save(state)` keeps it again.
 * `transparent` draws no background on the canvas; its parent shows the theme's color instead.
 * Returns `{ plot(msg), setMode(mode) }`: `plot` takes `{ rows, labels, cleared }`.
 */
export function mountPlotter({ saved = {}, save = () => {}, transparent = false } = {}) {
  const $ = (id) => document.getElementById(id);
  const canvas = $("canvas");
  const ctx = canvas.getContext("2d");

  // Saved themes before themeV 2 hold Radar's old light default, not a choice: drop it.
  const savedTheme = { ...(saved.theme ?? {}) };
  if (saved.themeV !== 2) delete savedTheme.radar;
  const view = {
    mode: MODES.includes(saved.mode) ? saved.mode : "time",
    theme: { time: "dark", xy: "dark", radar: "dark", ...savedTheme },
    hidden: new Set(saved.hidden ?? []),
  };
  let rows = [];
  let labels = [];
  let dirty = true;
  /** "auto" waits for the first labels to pick Radar, XY or Time. */
  let autoMode = false;

  const keep = () => save({ mode: view.mode, theme: view.theme, themeV: 2, hidden: [...view.hidden] });

  function fit() {
    const r = canvas.getBoundingClientRect();
    const d = window.devicePixelRatio || 1;
    const w = Math.max(1, Math.round(r.width * d)), h = Math.max(1, Math.round(r.height * d));
    if (canvas.width !== w || canvas.height !== h) { canvas.width = w; canvas.height = h; }
    ctx.setTransform(d, 0, 0, d, 0, 0);
    return { W: r.width, H: r.height };
  }

  function syncTabs() {
    const pairs = plotPairs(labels);
    const has = { time: labels.length > 0, xy: pairs.xy.length > 0, radar: pairs.radar.length > 0 };
    for (const b of document.querySelectorAll("#tabs button")) {
      b.classList.toggle("on", b.dataset.m === view.mode);
      b.classList.toggle("lit", has[b.dataset.m]);
    }
  }

  function syncLegend() {
    const th = THEMES[view.theme[view.mode]];
    const el = $("legend");
    el.replaceChildren();
    for (const k of labels) {
      const s = document.createElement("span");
      s.className = "leg-item" + (view.hidden.has(k) ? " off" : "");
      s.style.color = colorOf(labels, k, th, view.mode !== "time");
      s.textContent = "█ " + k;
      s.dataset.k = k;
      el.append(s);
    }
    const bg = document.createElement("span");
    bg.className = "leg-bg";
    bg.textContent = "bg";
    bg.title = "Background for this mode";
    el.append(bg);
  }

  function show(mode) {
    view.mode = mode;
    keep(); syncTabs(); syncLegend(); dirty = true;
  }

  /** A mode, or "auto": follow Radar, then XY, then Time as pairs appear, until a tab is clicked. */
  function setMode(mode) {
    autoMode = mode === "auto";
    if (autoMode) {
      if (labels.length) show(bestMode(labels));
    } else if (MODES.includes(mode)) {
      show(mode);
    }
  }

  function draw() {
    const { W, H } = fit();
    if (transparent) canvas.parentElement.style.background = THEMES[view.theme[view.mode]].bg;
    drawPlot(ctx, W, H, view.mode, { rows, labels, now: Date.now() }, { theme: view.theme[view.mode], hidden: view.hidden, winMs: WIN_MS, transparent });
  }

  // ~20 frames/s while lines arrive or points are still scrolling or fading; idle otherwise.
  let lastFrame = 0;
  function frame(ts) {
    requestAnimationFrame(frame);
    if (document.hidden || ts - lastFrame < 50) return;
    const lastT = rows.length ? rows[rows.length - 1].t : 0;
    const moving = Date.now() - lastT < (view.mode === "time" ? WIN_MS : FADE_MS) + 500;
    if (!dirty && !moving) return;
    lastFrame = ts;
    dirty = false;
    draw();
  }
  requestAnimationFrame(frame);
  new ResizeObserver(() => { dirty = true; }).observe(canvas);

  $("tabs").addEventListener("click", (e) => {
    const m = e.target.closest("button")?.dataset.m;
    if (m) setMode(m);
  });
  $("legend").addEventListener("click", (e) => {
    const t = e.target.closest("span");
    if (!t) return;
    if (t.classList.contains("leg-bg")) {
      view.theme[view.mode] = view.theme[view.mode] === "dark" ? "light" : "dark";
    } else if (t.dataset.k) {
      // A Plot Pair is one trace: its two labels show and hide together.
      const show = view.hidden.has(t.dataset.k);
      for (const k of pairOf(labels, t.dataset.k)) {
        if (show) view.hidden.delete(k);
        else view.hidden.add(k);
      }
    }
    keep(); syncLegend(); dirty = true;
  });

  function plot(msg) {
    if (msg.cleared) rows = [];
    for (const r of msg.rows) rows.push(r);
    const cut = Date.now() - KEEP_MS;
    let drop = 0;
    while (drop < rows.length && (rows.length - drop > MAX_ROWS || rows[drop].t < cut)) drop++;
    if (drop) rows.splice(0, drop);
    if (msg.labels.join("\n") !== labels.join("\n")) {
      labels = msg.labels;
      if (autoMode && labels.length && bestMode(labels) !== view.mode) view.mode = bestMode(labels);
      syncTabs(); syncLegend();
    }
    dirty = true;
  }

  syncTabs();
  syncLegend();
  return { plot, setMode };
}
