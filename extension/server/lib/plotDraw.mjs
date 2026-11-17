/**
 * plotDraw.mjs
 * ────────────
 * Draws one Plot Mode (Time, XY or Radar) from PlotBuffer rows (ADR 0009).
 * The VS Code webview and the MCP App pass a browser canvas context; the
 * plotSnapshot PNG passes the small software context in pngCanvas.mjs. Only
 * the canvas calls that context implements are used here.
 *
 *   - Time:  each label is a line over the last `winMs`.
 *   - XY:    each `_x`/`_y` pair is a trail fading over 5 s, equal scale on both axes.
 *   - Radar: 0° up, + clockwise; each degree keeps its latest point, fading over 5 s.
 */

import { FADE_MS, plotPairs, polarPoints, xyTrail } from "./plot.mjs";

const PAL_DARK = ["#7dff9e", "#ff7d9e", "#9e7dff", "#ffff7d", "#7dffff", "#ffb47d", "#ff9e7d", "#7db4ff", "#ff7dff", "#d4ff7d"];
const PAL_LIGHT = ["#1f8a4c", "#d1406b", "#6a4fd6", "#b08900", "#0a8fa8", "#d9731a", "#c0392b", "#2f6fd6", "#b03fb0", "#6c8a12"];

/** dark = the green Plotter, every mode's default; light = white with grey rings (the bg toggle). */
export const THEMES = {
  dark: { bg: "#020502", grid: "rgba(62,255,120,0.12)", ring: "rgba(62,255,120,0.18)", axis: "rgba(80,255,140,0.35)", text: "#8de0a0", pal: PAL_DARK, glow: true },
  light: { bg: "#ffffff", grid: "#ececec", ring: "#d6d6d6", axis: "#bdbdbd", text: "#707070", pal: PAL_LIGHT, glow: false },
};
export const MODES = ["time", "xy", "radar"];
export const MONO = "11px Consolas, Menlo, Monaco, monospace";

export function defaultTheme(_mode) {
  return "dark";
}

/** The mode that shows the most: Radar, then XY, then Time. */
export function bestMode(labels) {
  const p = plotPairs(labels);
  return p.radar.length ? "radar" : p.xy.length ? "xy" : "time";
}

/**
 * The label that leads a Plot Pair: name_y → name_x, name_cm/_mm/_m → name_deg
 * (when the partner is printed too); any other label leads itself.
 */
export function pairLead(labels, label) {
  const m = String(label).match(/^(.+)_(y|cm|mm|m)$/);
  if (!m) return label;
  const lead = `${m[1]}_${m[2] === "y" ? "x" : "deg"}`;
  return labels.includes(lead) ? lead : label;
}

/** Both labels of a Plot Pair, or just this one. */
export function pairOf(labels, label) {
  const lead = pairLead(labels, label);
  return labels.filter((k) => k === label || k === lead || pairLead(labels, k) === lead);
}

/**
 * A label's color: its place in the label list, so every mode and the legend
 * agree. In XY and Radar the two labels of a Plot Pair share one color, like
 * their one trace; Time draws each label as its own line (paired = false).
 */
export function colorOf(labels, label, theme, paired = true) {
  const i = Math.max(0, labels.indexOf(paired ? pairLead(labels, label) : label));
  return theme.pal[i % theme.pal.length];
}

function nice(x) {
  const p = Math.pow(10, Math.floor(Math.log10(x)));
  const f = x / p;
  return (f < 1.5 ? 1 : f < 3 ? 2 : f < 7 ? 5 : 10) * p;
}
function ticks(lo, hi, n) {
  const step = nice((hi - lo) / n);
  const a = Math.floor(lo / step) * step;
  const b = Math.ceil(hi / step) * step;
  const vals = [];
  for (let x = a; x <= b + step / 2; x += step) vals.push(+x.toFixed(10));
  return { lo: a, hi: b, vals };
}
export function fmt(v) {
  const a = Math.abs(v);
  return a >= 1000 ? v.toFixed(0) : a >= 100 ? v.toFixed(0) : a >= 10 ? v.toFixed(1) : v.toFixed(2);
}

function message(ctx, W, H, th, text) {
  ctx.fillStyle = th.text;
  ctx.font = MONO;
  ctx.textAlign = "center";
  ctx.fillText(text, W / 2, H / 2);
  ctx.textAlign = "left";
}

/**
 * Draw `mode` into a W×H area.
 * @param data `{ rows, labels, now }` (rows as PlotBuffer keeps them; now in ms)
 * @param opts `{ theme: "dark"|"light", hidden: Set<label>, winMs, transparent }`;
 *   `transparent` leaves the background out, for a surface that shows something behind the plot
 */
export function drawPlot(ctx, W, H, mode, data, opts = {}) {
  const th = THEMES[opts.theme ?? defaultTheme(mode)];
  const hidden = opts.hidden ?? new Set();
  ctx.globalAlpha = 1;
  if (opts.transparent) {
    ctx.clearRect(0, 0, W, H);
  } else {
    ctx.fillStyle = th.bg;
    ctx.fillRect(0, 0, W, H);
  }
  if (mode === "xy") drawXY(ctx, W, H, data, th, hidden);
  else if (mode === "radar") drawPolar(ctx, W, H, data, th, hidden);
  else drawTime(ctx, W, H, data, th, hidden, opts.winMs ?? 20000);
}

function drawTime(ctx, W, H, { rows, labels, now }, th, hidden, win) {
  const L = 50, R = W - 14, T = 16, B = H - 24;
  const shown = labels.filter((k) => !hidden.has(k));
  if (!shown.length) return message(ctx, W, H, th, labels.length ? "All series hidden" : "Waiting for label:value lines");
  let lo = Infinity, hi = -Infinity;
  for (const r of rows) {
    if (now - r.t > win) continue;
    for (const k of shown) {
      const v = r.v[k];
      if (v !== undefined) { lo = Math.min(lo, v); hi = Math.max(hi, v); }
    }
  }
  if (!Number.isFinite(lo)) { lo = 0; hi = 1; }
  if (hi - lo < 1e-9) { lo -= 1; hi += 1; }
  const tk = ticks(lo, hi, 5);
  lo = tk.lo; hi = tk.hi;
  const X = (t) => R - ((now - t) / win) * (R - L);
  const Y = (v) => B - ((v - lo) / (hi - lo)) * (B - T);

  ctx.strokeStyle = th.grid; ctx.lineWidth = 1; ctx.fillStyle = th.text; ctx.font = MONO;
  for (const v of tk.vals) {
    const y = Y(v);
    ctx.beginPath(); ctx.moveTo(L, y); ctx.lineTo(R, y); ctx.stroke();
    ctx.textAlign = "right"; ctx.fillText(fmt(v), L - 6, y + 4);
  }
  const stepMs = win > 30000 ? 10000 : 5000;
  for (let s = 0; s <= win; s += stepMs) {
    const x = R - (s / win) * (R - L);
    ctx.beginPath(); ctx.moveTo(x, T); ctx.lineTo(x, B); ctx.stroke();
    ctx.textAlign = "center"; ctx.fillText(s ? `-${s / 1000} s` : "now", x, B + 16);
  }
  ctx.textAlign = "left";
  ctx.strokeStyle = th.axis; ctx.lineWidth = 1.5;
  ctx.beginPath(); ctx.moveTo(L, T); ctx.lineTo(L, B); ctx.lineTo(R, B); ctx.stroke();

  ctx.save();
  ctx.beginPath(); ctx.rect(L, T - 2, R - L, B - T + 4); ctx.clip();
  ctx.lineWidth = 2; ctx.lineJoin = "round";
  if (th.glow) { ctx.shadowColor = "rgba(140,255,170,0.5)"; ctx.shadowBlur = 8; }
  for (const k of shown) {
    ctx.strokeStyle = colorOf(labels, k, th, false);
    ctx.beginPath();
    let started = false;
    for (const r of rows) {
      if (now - r.t > win + 500) continue;
      const v = r.v[k];
      if (v === undefined) { started = false; continue; }
      if (started) ctx.lineTo(X(r.t), Y(v)); else { ctx.moveTo(X(r.t), Y(v)); started = true; }
    }
    ctx.stroke();
  }
  ctx.restore();
}

function drawXY(ctx, W, H, { rows, labels, now }, th, hidden) {
  const names = plotPairs(labels).xy.filter((n) => !hidden.has(`${n}_x`));
  const trails = names.map((n) => ({ n, pts: xyTrail(rows, n, now) })).filter((t) => t.pts.length);
  if (!trails.length) return message(ctx, W, H, th, names.length ? "No XY points in the last 5 s" : "Print name_x and name_y on one line for XY");
  let x0 = Infinity, x1 = -Infinity, y0 = Infinity, y1 = -Infinity;
  for (const { pts } of trails) for (const p of pts) {
    x0 = Math.min(x0, p.x); x1 = Math.max(x1, p.x); y0 = Math.min(y0, p.y); y1 = Math.max(y1, p.y);
  }
  const span = Math.max(x1 - x0, y1 - y0, 1) * 1.15;
  const cx = (x0 + x1) / 2, cy = (y0 + y1) / 2;
  const s = Math.min(W - 60, H - 60) / span;
  const PX = (x) => W / 2 + (x - cx) * s, PY = (y) => H / 2 - (y - cy) * s;
  const step = nice(70 / s); // at least 70 px between grid labels
  const gx0 = cx - W / 2 / s, gx1 = cx + W / 2 / s, gy0 = cy - H / 2 / s, gy1 = cy + H / 2 / s;

  ctx.strokeStyle = th.grid; ctx.lineWidth = 1; ctx.fillStyle = th.text; ctx.font = MONO;
  for (let x = Math.ceil(gx0 / step) * step; x <= gx1; x += step) {
    const px = PX(x);
    ctx.beginPath(); ctx.moveTo(px, 0); ctx.lineTo(px, H); ctx.stroke();
    if (px + 44 < W) ctx.fillText(fmt(x), px + 3, H - 6);
  }
  for (let y = Math.ceil(gy0 / step) * step; y <= gy1; y += step) {
    const py = PY(y);
    ctx.beginPath(); ctx.moveTo(0, py); ctx.lineTo(W, py); ctx.stroke();
    if (Math.abs(py - (H - 6)) > 14) ctx.fillText(fmt(y), 4, py - 3);
  }
  ctx.strokeStyle = th.axis; ctx.lineWidth = 1.3;
  if (gx0 < 0 && gx1 > 0) { ctx.beginPath(); ctx.moveTo(PX(0), 0); ctx.lineTo(PX(0), H); ctx.stroke(); }
  if (gy0 < 0 && gy1 > 0) { ctx.beginPath(); ctx.moveTo(0, PY(0)); ctx.lineTo(W, PY(0)); ctx.stroke(); }

  for (const { n, pts } of trails) {
    const col = colorOf(labels, `${n}_x`, th);
    ctx.strokeStyle = col; ctx.lineWidth = 2; ctx.lineCap = "round";
    for (let i = 1; i < pts.length; i++) {
      ctx.globalAlpha = Math.max(0, 1 - (now - pts[i].t) / FADE_MS);
      ctx.beginPath(); ctx.moveTo(PX(pts[i - 1].x), PY(pts[i - 1].y)); ctx.lineTo(PX(pts[i].x), PY(pts[i].y)); ctx.stroke();
    }
    ctx.globalAlpha = 1;
    const last = pts[pts.length - 1];
    ctx.fillStyle = col;
    ctx.beginPath(); ctx.arc(PX(last.x), PY(last.y), 5, 0, Math.PI * 2); ctx.fill();
    ctx.fillStyle = th.text;
    ctx.fillText(`${n} (${fmt(last.x)}, ${fmt(last.y)})`, PX(last.x) + 9, PY(last.y) - 8);
  }
}

function drawPolar(ctx, W, H, { rows, labels, now }, th, hidden) {
  const pairs = plotPairs(labels).radar.filter((p) => !hidden.has(`${p.name}_deg`));
  const sets = pairs.map((p) => ({ p, ...polarPoints(rows, p, now) }));
  let maxCm = 0;
  for (const { byDeg } of sets) for (const q of byDeg.values()) maxCm = Math.max(maxCm, q.cm);
  const R = Math.min(W, H) / 2 - 28, cx = W / 2, cy = H / 2;
  const tk = ticks(0, Math.max(maxCm, 10), 4);
  const rMax = tk.hi;
  const P = (deg, cm) => {
    const a = (deg * Math.PI) / 180, r = (cm / rMax) * R;
    return [cx + r * Math.sin(a), cy - r * Math.cos(a)];
  };
  ctx.strokeStyle = th.ring; ctx.lineWidth = 1; ctx.fillStyle = th.text; ctx.font = MONO;
  for (const v of tk.vals) {
    if (v <= 0) continue;
    const r = (v / rMax) * R;
    ctx.beginPath(); ctx.arc(cx, cy, r, 0, Math.PI * 2); ctx.stroke();
    ctx.fillText(`${fmt(v)} cm`, cx + 4, cy - r + 12);
  }
  for (let d = 0; d < 360; d += 30) {
    const [x, y] = P(d, rMax);
    ctx.beginPath(); ctx.moveTo(cx, cy); ctx.lineTo(x, y); ctx.stroke();
    const [lx, ly] = P(d, rMax * 1.1);
    ctx.textAlign = "center"; ctx.fillText(`${d}°`, lx, ly + 4); ctx.textAlign = "left";
  }
  if (!pairs.length) return message(ctx, W, H + 60, th, "Print name_deg and name_cm on one line for Radar");
  for (const { p, byDeg } of sets) {
    ctx.fillStyle = colorOf(labels, `${p.name}_deg`, th);
    for (const [deg, q] of byDeg) {
      ctx.globalAlpha = Math.max(0, 1 - (now - q.t) / FADE_MS);
      const [x, y] = P(deg, Math.min(q.cm, rMax));
      ctx.beginPath(); ctx.arc(x, y, 3.2, 0, Math.PI * 2); ctx.fill();
    }
    ctx.globalAlpha = 1;
  }
}

/** One legend row: a colored square and the last value of each label (the PNG has no DOM legend). */
export function drawLegend(ctx, x, y, W, labels, last, themeName, mode = "time") {
  const th = THEMES[themeName];
  ctx.font = MONO;
  ctx.textAlign = "left";
  let cx = x;
  for (const k of labels) {
    const text = `${k} ${last[k] === undefined ? "-" : fmt(last[k])}`;
    const w = 14 + text.length * 7 + 14;
    if (cx + w > W) break;
    ctx.fillStyle = colorOf(labels, k, th, mode !== "time");
    ctx.fillRect(cx, y - 8, 8, 8);
    ctx.fillStyle = th.text;
    ctx.fillText(text, cx + 13, y);
    cx += w;
  }
}
