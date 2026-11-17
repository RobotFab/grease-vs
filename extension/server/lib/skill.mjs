/**
 * skill.mjs
 * ─────────
 * The Grease Skill text (server/SKILL.md), embedded at build time by the
 * esbuild text loader. The extension installs this copy into the agents'
 * skill folders, and AI Tether serves the same copy through `readSkill`, so
 * the two can never drift apart.
 */

import SKILL_TEXT from "../SKILL.md";

export { SKILL_TEXT };
