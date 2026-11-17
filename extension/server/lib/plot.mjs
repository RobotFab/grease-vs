/**
 * plot.mjs
 * ────────
 * The Plotter's data (ADR 0009). AI Tether parses every serial line into one
 * row of labeled numbers and keeps the rows in a rolling buffer. The Plotter
 * webview, readPlot and plotSnapshot each read rows after their own sequence
 * number, so none of them takes lines away from serialRead or each other.
 *
 * Lines: `label:value` or `label=value`; a line with no labels falls back to
 * bare numbers named ch1, ch2, …
 *
 * Plot Pairs (two labels on one line):
 *   - `<name>_x` + `<name>_y`   → XY
 *   - `<name>_deg` + `<name>_cm` → Radar (`_mm` and `_m` accepted, shown in cm)
 *
 * Plain JS with no Node imports: the webview bundles this file too.
 */

const NUM = "-?(?:\\d+\\.?\\d*|\\.\\d+)(?:[eE][-+]?\\d+)?";
const LABELED = new RegExp(`([A-Za-z_]\\w*)\\s*[:=]\\s*(${NUM})`, "g");
const BARE = new RegExp(NUM, "g");

/** Points older than this fade out of XY and Radar. */
export const FADE_MS = 5000;
/** Radar distance units, as a factor to cm. */
const TO_CM = { cm: 1, mm: 0.1, m: 100 };
/** More labels than this are ignored: a chatty sketch should not grow the legend forever. */
const MAX_LABELS = 24;

/** One serial line → `{ label: value }`, or null when it has no numbers. */
export function parsePlotLine(line) {
  const s = String(line);
  const v = {};
  let n = 0;
  for (const m of s.matchAll(LABELED)) {
    const x = Number(m[2]);
    if (Number.isFinite(x)) {
      v[m[1]] = x;
      n++;
    }
  }
  if (n) return v;
  let i = 0;
  for (const t of s.match(BARE) ?? []) {
    const x = Number(t);
    if (Number.isFinite(x)) v[`ch${++i}`] = x;
  }
  return i ? v : null;
}

/** The Plot Pairs among these labels. */
export function plotPairs(labels) {
  const set = new Set(labels);
  const xy = [];
  const radar = [];
  for (const k of labels) {
    let m = k.match(/^(.+)_x$/);
    if (m && set.has(`${m[1]}_y`)) xy.push(m[1]);
    m = k.match(/^(.+)_deg$/);
    if (m) {
      const unit = ["cm", "mm", "m"].find((u) => set.has(`${m[1]}_${u}`));
      if (unit) radar.push({ name: m[1], unit });
    }
  }
  return { xy, radar };
}

/** XY trail of one pair over the last FADE_MS, oldest first. */
export function xyTrail(rows, name, now, fadeMs = FADE_MS) {
  const out = [];
  const kx = `${name}_x`, ky = `${name}_y`;
  for (let i = rows.length - 1; i >= 0 && now - rows[i].t <= fadeMs; i--) {
    const v = rows[i].v;
    if (kx in v && ky in v) out.push({ x: v[kx], y: v[ky], t: rows[i].t });
  }
  return out.reverse();
}

/** Radar points of one pair: each whole degree keeps its latest distance (cm) within FADE_MS. */
export function polarPoints(rows, { name, unit }, now, fadeMs = FADE_MS) {
  const byDeg = new Map();
  let lastDeg = null;
  const kd = `${name}_deg`, kr = `${name}_${unit}`;
  for (let i = rows.length - 1; i >= 0 && now - rows[i].t <= fadeMs; i--) {
    const v = rows[i].v;
    if (!(kd in v && kr in v)) continue;
    const deg = Math.round(((v[kd] % 360) + 360) % 360) % 360;
    if (lastDeg === null) lastDeg = deg;
    if (!byDeg.has(deg)) byDeg.set(deg, { cm: v[kr] * TO_CM[unit], t: rows[i].t });
  }
  return { byDeg, lastDeg };
}

/** Rolling buffer of parsed rows: about 60 s, at most 6000 rows (100 lines/s). */
export class PlotBuffer {
  constructor({ maxRows = 6000, maxAgeMs = 60_000 } = {}) {
    this.maxRows = maxRows;
    this.maxAgeMs = maxAgeMs;
    /** Never reset, so a reader can tell new rows from old across a clear. */
    this.seq = 0;
    this.clearedAt = 0;
    this.rows = [];
    this.labels = [];
  }

  clear() {
    this.rows = [];
    this.labels = [];
    // A clear takes a seq of its own: every reader holding rows is behind it.
    this.clearedAt = ++this.seq;
  }

  push(line, t = Date.now()) {
    const parsed = parsePlotLine(line);
    if (!parsed) return;
    const v = {};
    for (const k in parsed) {
      if (!this.labels.includes(k)) {
        if (this.labels.length >= MAX_LABELS) continue;
        this.labels.push(k);
      }
      v[k] = parsed[k];
    }
    this.rows.push({ seq: ++this.seq, t, v });
    let drop = 0;
    while (drop < this.rows.length && (this.rows.length - drop > this.maxRows || t - this.rows[drop].t > this.maxAgeMs)) drop++;
    if (drop) this.rows.splice(0, drop);
  }

  /** Rows after `since` (a seq a reader got earlier). `cleared` tells it to drop what it holds. */
  since(since = 0) {
    const cleared = since < this.clearedAt || since > this.seq;
    const from = cleared ? 0 : since;
    let i = this.rows.length;
    while (i > 0 && this.rows[i - 1].seq > from) i--;
    return { seq: this.seq, cleared, labels: this.labels.slice(), rows: this.rows.slice(i) };
  }

  /** Rows from the last `seconds`. */
  window(seconds, now = Date.now()) {
    const from = now - seconds * 1000;
    let i = this.rows.length;
    while (i > 0 && this.rows[i - 1].t >= from) i--;
    return this.rows.slice(i);
  }

  /** Last, min, max and mean of each label over the last `seconds`, and the Plot Pairs. */
  summary(seconds = 10, now = Date.now()) {
    const rows = this.window(seconds, now);
    const labels = {};
    for (const r of rows) {
      for (const k in r.v) {
        const x = r.v[k];
        const s = (labels[k] ||= { last: x, min: x, max: x, sum: 0, n: 0 });
        s.last = x;
        s.min = Math.min(s.min, x);
        s.max = Math.max(s.max, x);
        s.sum += x;
        s.n++;
      }
    }
    const round = (x) => Number(x.toPrecision(6));
    for (const k in labels) {
      const s = labels[k];
      labels[k] = { last: round(s.last), min: round(s.min), max: round(s.max), mean: round(s.sum / s.n), samples: s.n };
    }
    return { seconds, rows: rows.length, labels, pairs: plotPairs(this.labels) };
  }
}
