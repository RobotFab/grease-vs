/**
 * plotterApp.mjs
 * ──────────────
 * The Plotter MCP App page (ui://grease/plotter), embedded at build time by
 * the esbuild text loader: scripts/build.mjs builds dist/plotter-app.html
 * from src/ui/plotterApp.html + plotterApp.mjs before this bundle.
 */

import PLOTTER_APP_HTML from "../../dist/plotter-app.html";

export const PLOTTER_APP_URI = "ui://grease/plotter";

export { PLOTTER_APP_HTML };
