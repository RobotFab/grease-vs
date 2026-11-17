#!/usr/bin/env node
/**
 * Pack the Claude Desktop bundle (.mcpb) from the same dist/ as the .vsix.
 * Run `npm run build` first. Claude Desktop starts the bridge on its own
 * Node.js, and the bridge starts AI Tether from the server.mjs beside it.
 */
import { execFileSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

const root = path.join(path.dirname(fileURLToPath(import.meta.url)), "..");
const pkg = JSON.parse(fs.readFileSync(path.join(root, "package.json"), "utf8"));
const output = path.join(root, `arduino-grease-${pkg.version}.mcpb`);
const mcpb = path.join(root, "node_modules", "@anthropic-ai", "mcpb", "dist", "cli", "cli.js");

const manifest = {
  manifest_version: "0.3",
  name: "grease",
  display_name: "Grease VS",
  version: pkg.version,
  description: "Agentic hardware debugging for ESP32, RP2040, nRF52840 and Arduino boards. The AI model compiles and uploads code, then reads and writes through Serial. Offers an XY Plotter, a Radar Plotter and a traditional Plotter.",
  author: { name: "RobotFab", url: "https://github.com/RobotFab" },
  repository: { type: "git", url: "https://github.com/RobotFab/grease-vs.git" },
  homepage: "https://robotfab.github.io/grease-vs/",
  support: "https://github.com/RobotFab/grease-vs/issues",
  license: "MIT",
  icon: "icon.png",
  server: {
    type: "node",
    entry_point: "server/grease-mcp.mjs",
    mcp_config: { command: "node", args: ["${__dirname}/server/grease-mcp.mjs"] },
  },
  tools_generated: true,
  compatibility: { platforms: ["darwin", "win32"], runtimes: { node: ">=18.0.0" } },
};

const stage = fs.mkdtempSync(path.join(os.tmpdir(), "grease-mcpb-"));
try {
  fs.mkdirSync(path.join(stage, "server"));
  for (const file of ["grease-mcp.mjs", "server.mjs"]) {
    fs.copyFileSync(path.join(root, "dist", file), path.join(stage, "server", file));
  }
  fs.copyFileSync(path.join(root, "icon.png"), path.join(stage, "icon.png"));
  fs.writeFileSync(path.join(stage, "manifest.json"), `${JSON.stringify(manifest, null, 2)}\n`);
  fs.rmSync(output, { force: true });
  execFileSync(process.execPath, [mcpb, "validate", path.join(stage, "manifest.json")], { stdio: "inherit" });
  execFileSync(process.execPath, [mcpb, "pack", stage, output], { stdio: "inherit" });
  console.log(`Packed ${path.basename(output)}`);
} finally {
  fs.rmSync(stage, { recursive: true, force: true });
}
