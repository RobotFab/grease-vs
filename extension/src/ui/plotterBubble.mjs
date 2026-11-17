/**
 * ui/plotterBubble.mjs
 * ────────────────────
 * Bubble-up behind the Plotter in the MCP App. Same glyphs, motion and states
 * as the sidebar (views/toolbarView.ts), without the collisions with text.
 * It stays discreet: the page shows this canvas at opacity 0.5, half the
 * sidebar's brightness. Glyph trails build up alpha over frames, so the
 * brightness is halved on the finished canvas and not per glyph.
 */

const CHARS = "abcdefghijklmnopqrstuvwxyzNH01+=-./\\[]{}()?!<>:;".split("");
const AMBER = ["#ffb142", "#ffa502", "#f6b93b", "#fdcb6e", "#e1a233", "#ffbe76"];
const LIFE = 40000;
const EXHAUST = { compile: 0.9, upload: 3, agentCompile: 0.9, agentUpload: 3 };
const SPEED = { compile: 0.5, upload: 1, agentCompile: 0.5, agentUpload: 1 };
const FLOATER_MS = { idle: 2200, agent: 700 };
const RISE = 1.8;
const THRUST_RISE = 2.2;
const SPREAD_PX = 30;
const MAX_GLYPHS = 400;
const FONT = "bold 12px Consolas, Menlo, monospace";
const AGENT = new Set(["agent", "agentCompile", "agentUpload"]);

const rnd = (a, b) => a + Math.random() * (b - a);
const pick = (a) => a[(Math.random() * a.length) | 0];

function colorOf(p, state) {
  if (AGENT.has(state)) return p.amber || (p.amber = pick(AMBER));
  if (state === "compile" || state === "upload") return "#55efc4";
  return "#7af0b5";
}

/** Runs on `canvas`. Returns `{ setState(state), setEnabled(on) }`. */
export function mountBubble(canvas, { enabled = true } = {}) {
  const ctx = canvas.getContext("2d");
  const reducedMotion = matchMedia("(prefers-reduced-motion: reduce)").matches;
  let on = enabled && !reducedMotion;
  let state = "idle";
  let W = 0, H = 0, parts = [], lastFloater = 0, exhaust = 0, frames = 0;

  function fit() {
    const r = canvas.getBoundingClientRect(), d = window.devicePixelRatio || 1;
    W = r.width; H = r.height;
    canvas.width = Math.max(1, Math.round(W * d));
    canvas.height = Math.max(1, Math.round(H * d));
    ctx.setTransform(d, 0, 0, d, 0, 0);
  }
  new ResizeObserver(fit).observe(canvas);
  fit();

  const spawn = (p) => parts.push({ vx: 0, born: performance.now(), ch: pick(CHARS), ...p });

  function step(now) {
    requestAnimationFrame(step);
    if (!on || document.hidden || !W) return;
    ctx.save();
    ctx.globalCompositeOperation = "destination-out";
    // A stronger fade now and then: 8-bit alpha never fades to 0 at 0.3, which leaves streaks.
    ctx.fillStyle = ++frames % 30 ? "rgba(0,0,0,0.3)" : "rgba(0,0,0,0.6)";
    ctx.fillRect(0, 0, W, H);
    ctx.restore();
    ctx.textBaseline = "middle";
    ctx.font = FONT;

    const gap = FLOATER_MS[state] ?? FLOATER_MS.idle;
    if (now - lastFloater > gap * rnd(0.5, 1.5)) {
      lastFloater = now;
      spawn({ x: rnd(8, W - 8), y: H - 6, vy: -0.4 });
    }
    const speed = SPEED[state] ?? 0;
    exhaust += EXHAUST[state] ?? 0;
    while (exhaust >= 1) {
      exhaust -= 1;
      spawn({ x: W / 2 + rnd(-SPREAD_PX, SPREAD_PX), y: H - 4, vx: rnd(-0.3, 0.3), vy: rnd(-6, -3.5) * speed, life: 4000 });
    }

    const rise = speed ? Math.max(RISE, THRUST_RISE * RISE * speed) : RISE;
    const alive = [];
    for (const p of parts) {
      const age = now - p.born, life = p.life ?? LIFE;
      if (age > life) continue;
      if (age > 1000) { p.vx += rnd(-0.005, 0.005); p.vy += rnd(-0.01, 0.01); }
      if (p.vy > -rise) p.vy = Math.max(-rise, p.vy - 0.01);
      else if (p.vy < -rise * 1.2) p.vy += 0.04;
      p.vx *= 0.99;
      p.x += p.vx; p.y += p.vy;
      if (p.x < -10 || p.x > W + 10 || p.y < -16) continue;

      let a = 0.9;
      if (life - age < 6000) a *= (life - age) / 6000;
      if (state === "idle") a *= 0.5;
      ctx.globalAlpha = Math.max(0, Math.min(1, a));
      ctx.fillStyle = colorOf(p, state);
      ctx.fillText(p.ch, p.x, p.y);
      alive.push(p);
    }
    ctx.globalAlpha = 1;
    parts = alive.length > MAX_GLYPHS ? alive.slice(alive.length - MAX_GLYPHS) : alive;
  }
  requestAnimationFrame(step);

  return {
    setState(s) { state = s; },
    setEnabled(v) {
      on = v && !reducedMotion;
      if (!on) { parts = []; ctx.clearRect(0, 0, W, H); }
    },
  };
}
