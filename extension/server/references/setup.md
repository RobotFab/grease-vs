# arduino-cli Installation

## Install

| OS | Command |
|----|---------|
| macOS | `brew install arduino-cli` |
| Linux | `curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh \| sh` |
| Windows | `winget install ArduinoSA.ArduinoCLI` |

After install, restart your IDE so the updated PATH is picked up.

## Arduino IDE 2.x bundled binary (macOS)

```bash
export ARDUINO_CLI_PATH="/Applications/Arduino IDE.app/Contents/Resources/app/node_modules/arduino-ide-extension/build/arduino-cli"
# Check:
find /Applications -name "arduino-cli" 2>/dev/null
```

## Linux: serial port permissions

If `POST /serial/open` returns a permission-denied error:

```bash
sudo usermod -a -G dialout $USER
# Log out and back in — the group change only takes effect on a new session.
```

## Linux: what the Setup check warns about

The Help panel lists these with the command to fix each one.

- **Flatpak editor:** cannot see USB ports or host programs. Install the editor from its .deb, .rpm or .tar.gz.
- **ModemManager:** probes new USB ports for about 10 s and can break 32U4 uploads. Disable it unless you use a USB cellular modem.
- **brltty:** takes CH340 USB serial ports. Mask `brltty-udev.service` unless you use a Braille display.
- **RP2040/RP2350:** picotool needs a udev rule for USB vendor `2e8a`.

## WSL2 and ChromeOS

WSL2 has no USB of its own. Share the board from Windows with `usbipd-win`, or run Grease on Windows itself.
On ChromeOS (Crostini), allow each USB device under Settings, Developers, Linux, Manage USB devices.

## Custom binary path

Set `ARDUINO_CLI_PATH` in your environment to point to any arduino-cli binary not on PATH. The Arduino Grease server reads this variable on startup.
