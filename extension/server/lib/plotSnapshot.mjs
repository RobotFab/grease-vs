/**
 * plotSnapshot.mjs
 * ────────────────
 * The Plotter as a PNG, for agents that show images but not MCP Apps (Codex).
 * plotDraw.mjs draws into PngCanvas, a software stand-in for the few canvas
 * calls it uses; the PNG is encoded with Node's zlib. No native canvas
 * module (ADR 0009).
 *
 * Text is a 5×7 bitmap font, upper case only: enough for axis numbers and labels.
 */

import zlib from "node:zlib";
import { bestMode, defaultTheme, drawLegend, drawPlot, THEMES } from "./plotDraw.mjs";

// prettier-ignore
const FONT = {
  "0": [14,17,19,21,25,17,14], "1": [4,12,4,4,4,4,14], "2": [14,17,1,2,4,8,31], "3": [31,2,4,2,1,17,14],
  "4": [2,6,10,18,31,2,2], "5": [31,16,30,1,1,17,14], "6": [6,8,16,30,17,17,14], "7": [31,1,2,4,8,8,8],
  "8": [14,17,17,14,17,17,14], "9": [14,17,17,15,1,2,12],
  A: [14,17,17,17,31,17,17], B: [30,17,17,30,17,17,30], C: [14,17,16,16,16,17,14], D: [28,18,17,17,17,18,28],
  E: [31,16,16,30,16,16,31], F: [31,16,16,30,16,16,16], G: [14,17,16,23,17,17,15], H: [17,17,17,31,17,17,17],
  I: [14,4,4,4,4,4,14], J: [7,2,2,2,2,18,12], K: [17,18,20,24,20,18,17], L: [16,16,16,16,16,16,31],
  M: [17,27,21,21,17,17,17], N: [17,17,25,21,19,17,17], O: [14,17,17,17,17,17,14], P: [30,17,17,30,16,16,16],
  Q: [14,17,17,17,21,18,13], R: [30,17,17,30,20,18,17], S: [15,16,16,14,1,1,30], T: [31,4,4,4,4,4,4],
  U: [17,17,17,17,17,17,14], V: [17,17,17,17,17,10,4], W: [17,17,17,21,21,21,10], X: [17,17,10,4,10,17,17],
  Y: [17,17,17,10,4,4,4], Z: [31,1,2,4,8,16,31],
  ".": [0,0,0,0,0,12,12], ",": [0,0,0,0,12,4,8], "-": [0,0,0,31,0,0,0], ":": [0,12,12,0,12,12,0],
  "(": [2,4,8,8,8,4,2], ")": [8,4,2,2,2,4,8], "_": [0,0,0,0,0,0,31], "°": [12,18,18,12,0,0,0],
  "/": [0,1,2,4,8,16,0], "%": [24,25,2,4,8,19,3], "+": [0,4,4,31,4,4,0], "=": [0,0,31,0,31,0,0],
  " ": [0,0,0,0,0,0,0], "?": [14,17,1,2,4,0,4],
};

function parseColor(s) {
  const str = String(s).trim();
  let m = str.match(/^#([0-9a-f]{3})$/i);
  if (m) return [...m[1]].map((h) => parseInt(h + h, 16)).concat(1);
  m = str.match(/^#([0-9a-f]{6})([0-9a-f]{2})?$/i);
  if (m) return [0, 2, 4].map((i) => parseInt(m[1].slice(i, i + 2), 16)).concat(m[2] ? parseInt(m[2], 16) / 255 : 1);
  m = str.match(/^rgba?\(([^)]+)\)$/i);
  if (m) {
    const p = m[1].split(",").map((x) => Number(x.trim()));
    return [p[0], p[1], p[2], p[3] ?? 1];
  }
  return [0, 0, 0, 1];
}

const CRC_TABLE = Array.from({ length: 256 }, (_, n) => {
  let c = n;
  for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
  return c >>> 0;
});
function crc32(buf) {
  let c = 0xffffffff;
  for (const b of buf) c = CRC_TABLE[(c ^ b) & 0xff] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
}
function chunk(type, data) {
  const len = Buffer.alloc(4);
  len.writeUInt32BE(data.length);
  const td = Buffer.concat([Buffer.from(type, "ascii"), data]);
  const crc = Buffer.alloc(4);
  crc.writeUInt32BE(crc32(td));
  return Buffer.concat([len, td, crc]);
}

/** The canvas calls plotDraw.mjs makes, drawn into an RGB pixel buffer at `scale`× resolution. */
export class PngCanvas {
  constructor(W, H, scale = 2) {
    this.S = scale;
    this.w = Math.round(W * scale);
    this.h = Math.round(H * scale);
    this.px = new Uint8Array(this.w * this.h * 3);
    this.mask = new Float32Array(this.w * this.h);
    Object.assign(this, { fillStyle: "#000", strokeStyle: "#000", lineWidth: 1, globalAlpha: 1, font: "11px monospace", textAlign: "left" });
    // Accepted and ignored: no blur or joins in the PNG.
    Object.assign(this, { shadowColor: "", shadowBlur: 0, lineJoin: "miter", lineCap: "butt" });
    this.clipBox = [0, 0, this.w, this.h];
    this.stack = [];
    this.path = [];
  }

  save() {
    const { fillStyle, strokeStyle, lineWidth, globalAlpha, font, textAlign, clipBox } = this;
    this.stack.push({ fillStyle, strokeStyle, lineWidth, globalAlpha, font, textAlign, clipBox });
  }
  restore() {
    Object.assign(this, this.stack.pop() ?? {});
  }

  beginPath() { this.path = []; }
  moveTo(x, y) { this.path.push([[x * this.S, y * this.S]]); }
  lineTo(x, y) {
    if (!this.path.length) return this.moveTo(x, y);
    this.path[this.path.length - 1].push([x * this.S, y * this.S]);
  }
  arc(cx, cy, r, a0, a1) {
    const n = Math.max(12, Math.ceil(r * this.S * Math.abs(a1 - a0) / 3));
    const pts = [];
    for (let i = 0; i <= n; i++) {
      const a = a0 + ((a1 - a0) * i) / n;
      pts.push([(cx + r * Math.cos(a)) * this.S, (cy + r * Math.sin(a)) * this.S]);
    }
    if (this.path.length) this.path[this.path.length - 1].push(...pts);
    else this.path.push(pts);
  }
  rect(x, y, w, h) {
    this.moveTo(x, y); this.lineTo(x + w, y); this.lineTo(x + w, y + h); this.lineTo(x, y + h); this.lineTo(x, y);
  }
  clip() {
    const pts = this.path.flat();
    if (!pts.length) return;
    const xs = pts.map((p) => p[0]), ys = pts.map((p) => p[1]);
    const [a, b, c, d] = this.clipBox;
    this.clipBox = [Math.max(a, Math.min(...xs)), Math.max(b, Math.min(...ys)), Math.min(c, Math.max(...xs)), Math.min(d, Math.max(...ys))];
  }

  /** Mask pixels (coverage 0..1) are blended once per stroke or fill, so overlaps do not darken. */
  _blend(touched, color) {
    const [r, g, b, a] = parseColor(color);
    const k = a * this.globalAlpha;
    for (const i of touched) {
      const al = this.mask[i] * k;
      this.mask[i] = 0;
      if (al <= 0) continue;
      const j = i * 3;
      this.px[j] += (r - this.px[j]) * al;
      this.px[j + 1] += (g - this.px[j + 1]) * al;
      this.px[j + 2] += (b - this.px[j + 2]) * al;
    }
  }
  _box(x0, y0, x1, y1) {
    const [a, b, c, d] = this.clipBox;
    return [Math.max(a, Math.floor(x0)), Math.max(b, Math.floor(y0)), Math.min(c, Math.ceil(x1)), Math.min(d, Math.ceil(y1))];
  }

  stroke() {
    const half = Math.max(0.5, (this.lineWidth * this.S) / 2);
    const touched = [];
    for (const sub of this.path) {
      for (let s = 1; s < sub.length; s++) {
        const [ax, ay] = sub[s - 1], [bx, by] = sub[s];
        const [x0, y0, x1, y1] = this._box(Math.min(ax, bx) - half - 1, Math.min(ay, by) - half - 1, Math.max(ax, bx) + half + 1, Math.max(ay, by) + half + 1);
        const dx = bx - ax, dy = by - ay, len2 = dx * dx + dy * dy || 1e-9;
        for (let y = y0; y < y1; y++) {
          for (let x = x0; x < x1; x++) {
            const px = x + 0.5, py = y + 0.5;
            const t = Math.max(0, Math.min(1, ((px - ax) * dx + (py - ay) * dy) / len2));
            const ex = px - (ax + t * dx), ey = py - (ay + t * dy);
            const cov = Math.min(1, half + 0.5 - Math.sqrt(ex * ex + ey * ey));
            if (cov <= 0) continue;
            const i = y * this.w + x;
            if (this.mask[i] === 0) touched.push(i);
            if (cov > this.mask[i]) this.mask[i] = cov;
          }
        }
      }
    }
    this._blend(touched, this.strokeStyle);
  }

  fill() {
    const pts = this.path.flat();
    if (pts.length < 3) return;
    const xs = pts.map((p) => p[0]), ys = pts.map((p) => p[1]);
    const [x0, y0, x1, y1] = this._box(Math.min(...xs), Math.min(...ys), Math.max(...xs), Math.max(...ys));
    const touched = [];
    for (let y = y0; y < y1; y++) {
      for (let x = x0; x < x1; x++) {
        const px = x + 0.5, py = y + 0.5;
        let inside = false;
        for (const sub of this.path) {
          for (let i = 0, j = sub.length - 1; i < sub.length; j = i++) {
            const [xi, yi] = sub[i], [xj, yj] = sub[j];
            if (yi > py !== yj > py && px < ((xj - xi) * (py - yi)) / (yj - yi) + xi) inside = !inside;
          }
        }
        if (!inside) continue;
        const i = y * this.w + x;
        this.mask[i] = 1;
        touched.push(i);
      }
    }
    this._blend(touched, this.fillStyle);
  }

  fillRect(x, y, w, h) {
    const S = this.S;
    const [x0, y0, x1, y1] = this._box(Math.round(x * S), Math.round(y * S), Math.round((x + w) * S), Math.round((y + h) * S));
    const touched = [];
    for (let yy = y0; yy < y1; yy++) for (let xx = x0; xx < x1; xx++) {
      const i = yy * this.w + xx;
      this.mask[i] = 1;
      touched.push(i);
    }
    this._blend(touched, this.fillStyle);
  }

  fillText(text, x, y) {
    const size = Number(String(this.font).match(/([\d.]+)px/)?.[1] ?? 11);
    const g = Math.max(1, Math.round((size * this.S) / 11)); // device pixels per font cell
    const chars = [...String(text).toUpperCase()];
    const width = (chars.length * 6 - 1) * g;
    let left = Math.round(x * this.S - (this.textAlign === "center" ? width / 2 : this.textAlign === "right" ? width : 0));
    const top = Math.round(y * this.S) - 7 * g;
    const touched = [];
    for (const ch of chars) {
      const rows = FONT[ch] ?? FONT["?"];
      for (let r = 0; r < 7; r++) for (let c = 0; c < 5; c++) {
        if (!(rows[r] & (16 >> c))) continue;
        const [x0, y0, x1, y1] = this._box(left + c * g, top + r * g, left + (c + 1) * g, top + (r + 1) * g);
        for (let yy = y0; yy < y1; yy++) for (let xx = x0; xx < x1; xx++) {
          const i = yy * this.w + xx;
          if (this.mask[i] === 0) touched.push(i);
          this.mask[i] = 1;
        }
      }
      left += 6 * g;
    }
    this._blend(touched, this.fillStyle);
  }

  toPng() {
    const { w, h, px } = this;
    const raw = Buffer.alloc((w * 3 + 1) * h);
    for (let y = 0; y < h; y++) {
      raw[y * (w * 3 + 1)] = 0;
      Buffer.from(px.buffer, y * w * 3, w * 3).copy(raw, y * (w * 3 + 1) + 1);
    }
    const ihdr = Buffer.alloc(13);
    ihdr.writeUInt32BE(w, 0);
    ihdr.writeUInt32BE(h, 4);
    ihdr.set([8, 2, 0, 0, 0], 8); // 8-bit RGB
    return Buffer.concat([
      Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]),
      chunk("IHDR", ihdr),
      chunk("IDAT", zlib.deflateSync(raw, { level: 6 })),
      chunk("IEND", Buffer.alloc(0)),
    ]);
  }
}

/**
 * PNG of one Plot Mode from a PlotBuffer, 800×460 drawn at 2×, with a legend row.
 * @param {import("./plot.mjs").PlotBuffer} plot
 * @param {{ mode?: "auto"|"time"|"xy"|"radar", seconds?: number }} opts
 */
export function plotSnapshot(plot, { mode = "auto", seconds = 20 } = {}) {
  const W = 800, H = 460, LEGEND = 28;
  const now = Date.now();
  const labels = plot.labels.slice();
  const chosen = mode === "auto" ? bestMode(labels) : mode;
  const theme = defaultTheme(chosen);
  const c = new PngCanvas(W, H, 2);
  drawPlot(c, W, H - LEGEND, chosen, { rows: plot.window(Math.max(seconds, 5), now), labels, now }, { theme, winMs: seconds * 1000 });
  c.fillStyle = THEMES[theme].bg;
  c.fillRect(0, H - LEGEND, W, LEGEND);
  const last = plot.rows.length ? plot.rows[plot.rows.length - 1].v : {};
  drawLegend(c, 10, H - 10, W - 10, labels, last, theme, chosen);
  return { png: c.toPng(), mode: chosen };
}
