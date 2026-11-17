/**
 * linuxChecks.mjs
 *
 * Read-only checks for what commonly stops boards on Linux. The Help panel
 * shows each one with its fix; nothing here changes the computer. Other
 * operating systems get an empty list.
 *
 * `blocking` problems count in the panel's "to fix". The others only cost
 * some boards, so they stay in Details as warnings.
 */

import { execFile } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";

const UDEV_DIRS = ["/etc/udev/rules.d", "/usr/lib/udev/rules.d", "/lib/udev/rules.d"];
const RP_VENDOR = /2e8a/i;
const RP_RULE = `echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", MODE="0666"' | sudo tee /etc/udev/rules.d/60-picotool.rules && sudo udevadm control --reload-rules`;

/** Standard output of a short command; "" when it is missing or times out. */
function runQuiet(command, args) {
  return new Promise((resolve) => {
    execFile(command, args, { timeout: 3000 }, (_err, stdout) => resolve(`${stdout ?? ""}`.trim()));
  });
}

function hasRpRule(dirs = UDEV_DIRS) {
  for (const dir of dirs) {
    let names = [];
    try {
      names = fs.readdirSync(dir);
    } catch {
      continue;
    }
    for (const name of names) {
      try {
        if (name.endsWith(".rules") && RP_VENDOR.test(fs.readFileSync(path.join(dir, name), "utf8"))) return true;
      } catch {
        /* an unreadable rule file says nothing either way */
      }
    }
  }
  return false;
}

const realSystem = () => ({
  platform: process.platform,
  isFlatpak: !!process.env.FLATPAK_ID || fs.existsSync("/.flatpak-info"),
  modemManagerState: () => runQuiet("systemctl", ["is-active", "ModemManager"]),
  brlttyUnitState: () => runQuiet("systemctl", ["is-enabled", "brltty-udev.service"]),
  rpCoreInstalled: fs.existsSync(path.join(os.homedir(), ".arduino15", "packages", "rp2040")),
  hasRpRule,
});

/**
 * @param {ReturnType<typeof realSystem>} [system] replaced in tests
 * @returns {Promise<{ id: string, blocking: boolean, title: string, why: string, fix?: string, note?: string }[]>}
 */
export async function linuxChecks(system = realSystem()) {
  if (system.platform !== "linux") return [];
  const found = [];

  if (system.isFlatpak) {
    found.push({
      id: "flatpak",
      blocking: true,
      title: "Flatpak editor",
      why: "A Flatpak editor cannot see USB ports or programs installed on this computer.",
      note: "Install the editor from its .deb, .rpm or .tar.gz instead.",
    });
  }

  const [modem, brltty] = await Promise.all([system.modemManagerState(), system.brlttyUnitState()]);

  if (modem === "active") {
    found.push({
      id: "modemmanager",
      blocking: false,
      title: "ModemManager running",
      why: "It probes new USB ports for about 10 s, which can break uploads to 32U4 boards such as Leonardo.",
      fix: "sudo systemctl disable --now ModemManager",
      note: "Skip this if you use a USB cellular modem.",
    });
  }

  if (brltty && brltty !== "masked" && brltty !== "not-found") {
    found.push({
      id: "brltty",
      blocking: false,
      title: "brltty installed",
      why: "It takes CH340 USB serial ports, so the board's port disappears.",
      fix: "sudo systemctl mask --now brltty-udev.service",
      note: "Skip this if you use a Braille display.",
    });
  }

  if (system.rpCoreInstalled && !system.hasRpRule()) {
    found.push({
      id: "udev-rp",
      blocking: false,
      title: "No USB rule for RP2040/RP2350",
      why: "Without a udev rule, picotool cannot reach the board to upload.",
      fix: RP_RULE,
    });
  }

  return found;
}
