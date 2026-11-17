#!/usr/bin/env node
import * as esbuild from "esbuild";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const root = path.join(__dirname, "..");
const watch = process.argv.includes("--watch");
const pkg = JSON.parse(fs.readFileSync(path.join(root, "package.json"), "utf8"));

// Files older builds left in dist/ that nothing reads any more. The skill is
// embedded in both bundles now, so a stale dist/SKILL.md cannot ship again.
for (const stale of ["SKILL.md", "server.mjs.orig", "extension.js.map"]) {
  if (watch && stale.endsWith(".map")) continue;
  fs.rmSync(path.join(root, "dist", stale), { force: true });
}

const buildStamp = String(Date.now());

const sharedConfig = {
  bundle: true,
  sourcemap: !watch ? false : "linked",
  minify: !watch,
  logLevel: "info",
  // server/SKILL.md is imported as text by server/lib/skill.mjs, and
  // dist/plotter-app.html by server/lib/plotterApp.mjs.
  loader: { ".md": "text", ".html": "text" },
  define: { GREASE_VERSION: JSON.stringify(pkg.version), GREASE_BUILD: JSON.stringify(buildStamp) },
};

// ── Extension host bundle (CJS, runs inside VS Code) ───────────────────────
const extensionCtx = await esbuild.context({
  ...sharedConfig,
  entryPoints: [path.join(root, "src/extension.ts")],
  outfile: path.join(root, "dist/extension.js"),
  format: "cjs",
  platform: "node",
  external: ["vscode"],
});

// ── MCP server bundle (ESM, runs as a standalone Node.js child process) ────
// createRequire banner lets inlined CJS deps call require() for Node built-ins
// without breaking ESM mode on Node v22+.
const serverCtx = await esbuild.context({
  ...sharedConfig,
  entryPoints: [path.join(root, "server/server.mjs")],
  outfile: path.join(root, "dist/server.mjs"),
  format: "esm",
  platform: "node",
  banner: {
    js: "import { createRequire } from 'module'; const require = createRequire(import.meta.url);",
  },
});

// ── Plotter webview script (browser IIFE). It shares server/lib/plot.mjs and
// plotDraw.mjs with AI Tether, so the panel and plotSnapshot draw the same way.
const plotterCtx = await esbuild.context({
  ...sharedConfig,
  entryPoints: [path.join(root, "src/ui/plotterWebview.mjs")],
  outfile: path.join(root, "dist/plotter.js"),
  format: "iife",
  platform: "browser",
});

// ── Plotter MCP App (ui://grease/plotter): the bundled script goes inline
// into src/ui/plotterApp.html, because hosts load the page as one resource.
// AI Tether embeds the result, so this builds before the server bundle.
const appHtml = path.join(root, "dist/plotter-app.html");
const plotterAppCtx = await esbuild.context({
  ...sharedConfig,
  sourcemap: false,
  entryPoints: [path.join(root, "src/ui/plotterApp.mjs")],
  outfile: path.join(root, "dist/plotter-app.js"),
  format: "esm",
  platform: "browser",
  write: false,
  plugins: [{
    name: "inline-into-html",
    setup(build) {
      build.onEnd((result) => {
        const js = result.outputFiles?.find((f) => f.path.endsWith(".js"));
        if (!js) return;
        const page = fs.readFileSync(path.join(root, "src/ui/plotterApp.html"), "utf8");
        const html = page.replace("/*PLOTTER_APP_JS*/", () => js.text.replaceAll("</script", "<\\/script"));
        fs.mkdirSync(path.dirname(appHtml), { recursive: true });
        if (!fs.existsSync(appHtml) || fs.readFileSync(appHtml, "utf8") !== html) fs.writeFileSync(appHtml, html);
      });
    },
  }],
});

// ── stdio bridge: copied as written (node: imports only), comments and all,
// because Developers may open ~/.grease/bin/grease-mcp.mjs to see what their
// agent runs. The extension copies it there on activation. Only its build
// stamp changes, so it can tell when a running AI Tether is older.
fs.mkdirSync(path.join(root, "dist"), { recursive: true });
const bridgeSource = fs.readFileSync(path.join(root, "server/bridge/grease-mcp.mjs"), "utf8");
if (!bridgeSource.includes('BRIDGE_BUILD = Number("0")')) throw new Error("grease-mcp.mjs: build stamp marker missing");
fs.writeFileSync(path.join(root, "dist/grease-mcp.mjs"), bridgeSource.replace('BRIDGE_BUILD = Number("0")', `BRIDGE_BUILD = Number("${buildStamp}")`));

const contexts = [extensionCtx, serverCtx, plotterCtx, plotterAppCtx];
await plotterAppCtx.rebuild();
if (watch) {
  await Promise.all(contexts.map((c) => c.watch()));
  console.log("Watching for changes…");
} else {
  await Promise.all(contexts.map((c) => c.rebuild()));
  await Promise.all(contexts.map((c) => c.dispose()));
}
