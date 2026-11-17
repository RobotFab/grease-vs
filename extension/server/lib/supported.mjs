/**
 * supported.mjs
 * ─────────────
 * What Grease installs, and how it recognizes a board offline (ADR 0005).
 *
 *   • SUPPORTED_CORES: the short list of board cores. Each carries its own
 *     index URL, passed to arduino-cli for one run only (--additional-urls);
 *     the global config is never written.
 *   • boardFromUsb(): VID:PID → core + FQBN, without the cloud lookup that
 *     `board list` would otherwise make.
 *   • parseGitHubLibrary(): the strict input parser for "Install library from
 *     GitHub" (Developer only; agents install registry libraries by name).
 */

/**
 * Newer boards first; the Arduino AVR family stays for the Romi 32U4 and the Uno.
 * Every official Arduino core comes from arduino-cli's built-in index (url null),
 * so adding one contacts no third-party server.
 */
export const SUPPORTED_CORES = Object.freeze([
  {
    id: "esp32:esp32",
    name: "ESP32 (C, S and P series)",
    vendor: "Espressif",
    url: "https://espressif.github.io/arduino-esp32/package_esp32_index.json",
    boards: ["ESP32-S3", "ESP32-C3", "ESP32-C6", "ESP32-P4", "XIAO ESP32-C6/S3/C3"],
  },
  {
    id: "rp2040:rp2040",
    name: "RP2350 and RP2040 (Pico 2, Pico, RP2350B boards)",
    vendor: "Earle Philhower",
    url: "https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json",
    boards: ["Pimoroni Pico Plus 2 (RP2350B, 8 MB PSRAM)", "Raspberry Pi Pico 2 / 2 W", "Raspberry Pi Pico / Pico W"],
  },
  {
    id: "adafruit:nrf52",
    name: "nRF52840 (Adafruit)",
    vendor: "Adafruit",
    url: "https://adafruit.github.io/arduino-board-index/package_adafruit_index.json",
    boards: ["Feather nRF52840 Express / Sense", "ItsyBitsy nRF52840"],
  },
  {
    id: "Seeeduino:nrf52",
    name: "Seeed XIAO nRF52840 / Sense",
    vendor: "Seeed Studio",
    url: "https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json",
    boards: ["XIAO nRF52840", "XIAO nRF52840 Sense"],
  },
  {
    id: "arduino:avr",
    name: "Arduino AVR (Leonardo, Uno R3, Nano, Mega 2560; Pololu Romi 32U4)",
    vendor: "Arduino",
    url: null, // built into arduino-cli's own index
    boards: ["Pololu Romi 32U4 (as Leonardo)", "Arduino Leonardo", "Arduino Uno R3", "Arduino Nano", "Arduino Mega 2560"],
  },
  {
    id: "arduino:renesas_uno",
    name: "Arduino Uno R4 (Minima, WiFi)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Uno R4 Minima", "Arduino Uno R4 WiFi"],
  },
  {
    id: "arduino:samd",
    name: "Arduino SAMD (Nano 33 IoT, MKR family, Zero)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Nano 33 IoT", "Arduino MKR WiFi 1010", "Arduino MKR Zero / 1000 / WAN / GSM / NB", "Arduino Zero"],
  },
  {
    id: "arduino:megaavr",
    name: "Arduino megaAVR (Nano Every, Uno WiFi Rev2)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Nano Every", "Arduino Uno WiFi Rev2"],
  },
  {
    id: "arduino:mbed_nano",
    name: "Arduino Nano 33 BLE and Nano RP2040 Connect (Mbed OS)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Nano 33 BLE / Sense", "Arduino Nano RP2040 Connect"],
  },
  {
    id: "arduino:sam",
    name: "Arduino Due (SAM, Cortex-M3)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Due"],
  },
  {
    id: "arduino:mbed_giga",
    name: "Arduino Giga R1 (Mbed OS)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Giga R1 WiFi"],
  },
  {
    id: "arduino:mbed_portenta",
    name: "Arduino Portenta H7 / X8 (Mbed OS)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Portenta H7", "Arduino Portenta X8"],
  },
  {
    id: "arduino:renesas_portenta",
    name: "Arduino Portenta C33",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Portenta C33"],
  },
  {
    id: "arduino:mbed_nicla",
    name: "Arduino Nicla (Sense ME, Vision, Voice)",
    vendor: "Arduino",
    url: null,
    boards: ["Nicla Sense ME", "Nicla Vision", "Nicla Voice"],
  },
  {
    id: "arduino:mbed_opta",
    name: "Arduino Opta (Mbed OS)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino Opta"],
  },
  {
    id: "arduino:zephyr",
    name: "Arduino UNO Q (Zephyr)",
    vendor: "Arduino",
    url: null,
    boards: ["Arduino UNO Q"],
  },
]);

export function supportedCore(id) {
  return SUPPORTED_CORES.find((c) => c.id === id) ?? null;
}

/**
 * The --additional-urls value for one install of `coreId`. An empty string
 * still overrides the config list, so a broken third-party URL in the
 * Developer's arduino-cli.yaml is never contacted while installing a
 * built-in core.
 */
export function additionalUrlsFor(coreId) {
  return supportedCore(coreId)?.url ?? "";
}

// ── USB identity → board, offline ─────────────────────────────────────────

/**
 * VID:PID (lowercase hex, no 0x) → the core a board needs and its FQBN.
 * `ask: true` marks IDs shared by several boards (USB-UART bridges,
 * Espressif's built-in USB, reused RP2040 IDs): the FQBN is only a
 * suggestion and Grease asks which board. arduino-cli matches installed
 * cores itself; this map covers boards whose core is missing, and boards no
 * core claims (Pololu Romi 32U4). Sources: boards.txt of avr 1.8.8,
 * esp32 3.3.12, rp2040 5.6.1, Pololu a-star 5.1.0, mbed_nano 4.5.0, samd 1.8.14, megaavr 1.8.8, sam 1.6.12,
 * renesas 1.6.0, mbed 4.6.0, zephyr 1.1.0 (checked 2026-11-10), and the
 * Adafruit / Seeed nRF52 repositories (checked 2026-11-06).
 */
const AVR = "arduino:avr";
const PICO = "rp2040:rp2040";
const ESP = "esp32:esp32";
const ADA = "adafruit:nrf52";
const SEEED = "Seeeduino:nrf52";
const MBED = "arduino:mbed_nano";
const SAMD = "arduino:samd";

/** [ids, core, fqbn (or the suggestion when ask), name, mode, ask] */
const USB_ROWS = [
  // Pololu A-Star / Romi / Zumo / Balboa 32U4: no core claims 1FFB, and Grease
  // always builds them as Leonardo (after one Grease upload they enumerate as 2341:8036).
  [["1ffb:2300"], AVR, "arduino:avr:leonardo", "Pololu Romi / A-Star 32U4", "app"],
  [["1ffb:0101"], AVR, "arduino:avr:leonardo", "Pololu Romi / A-Star 32U4 (bootloader, 8 s)", "bootloader"],
  // Arduino AVR
  [["2341:8036", "2a03:8036"], AVR, "arduino:avr:leonardo", "Arduino Leonardo (or a 32U4 built as Leonardo)", "app"],
  [["2341:0036", "2a03:0036"], AVR, "arduino:avr:leonardo", "Arduino Leonardo (bootloader)", "bootloader"],
  [["2341:8037", "2341:8237", "2a03:8037"], AVR, "arduino:avr:micro", "Arduino Micro", "app"],
  [["2341:0037", "2341:0237", "2a03:0037"], AVR, "arduino:avr:micro", "Arduino Micro (bootloader)", "bootloader"],
  [["2341:0043", "2341:0001", "2341:0243", "2341:006a", "2a03:0043"], AVR, "arduino:avr:uno", "Arduino Uno R3", "app"],
  [["2341:0042", "2341:0010", "2341:0242", "2341:0210", "2a03:0042", "2a03:0010"], AVR, "arduino:avr:mega", "Arduino Mega 2560", "app"],
  // Arduino Uno R4, Nano 33 BLE
  [["2341:0069", "2341:0369"], "arduino:renesas_uno", "arduino:renesas_uno:minima", "Arduino Uno R4 Minima", "app"],
  [["2341:1002", "2341:006d"], "arduino:renesas_uno", "arduino:renesas_uno:unor4wifi", "Arduino Uno R4 WiFi", "app"],
  [["2341:0074", "2341:0374"], "arduino:renesas_uno", "arduino:renesas_uno:nanor4", "Arduino Nano R4", "app"],
  [["2341:805a", "2341:015a", "2341:025a"], MBED, "arduino:mbed_nano:nano33ble", "Arduino Nano 33 BLE", "app"],
  [["2341:005a"], MBED, "arduino:mbed_nano:nano33ble", "Arduino Nano 33 BLE (bootloader)", "bootloader"],
  [["2341:805e", "2341:015e", "2341:025e"], MBED, "arduino:mbed_nano:nanorp2040connect", "Arduino Nano RP2040 Connect", "app"],
  [["2341:005e"], MBED, "arduino:mbed_nano:nanorp2040connect", "Arduino Nano RP2040 Connect (bootloader)", "bootloader"],
  // Arduino SAMD: 0x80xx is the sketch, 0x00xx the bootloader (double-tap reset)
  [["2341:8057"], SAMD, "arduino:samd:nano_33_iot", "Arduino Nano 33 IoT", "app"],
  [["2341:0057"], SAMD, "arduino:samd:nano_33_iot", "Arduino Nano 33 IoT (bootloader)", "bootloader"],
  [["2341:8054"], SAMD, "arduino:samd:mkrwifi1010", "Arduino MKR WiFi 1010", "app"],
  [["2341:0054"], SAMD, "arduino:samd:mkrwifi1010", "Arduino MKR WiFi 1010 (bootloader)", "bootloader"],
  [["2341:804f"], SAMD, "arduino:samd:mkrzero", "Arduino MKR Zero", "app"],
  [["2341:004f"], SAMD, "arduino:samd:mkrzero", "Arduino MKR Zero (bootloader)", "bootloader"],
  [["2341:804e", "2341:824e"], SAMD, "arduino:samd:mkr1000", "Arduino MKR 1000 WiFi", "app"],
  [["2341:004e", "2341:024e"], SAMD, "arduino:samd:mkr1000", "Arduino MKR 1000 WiFi (bootloader)", "bootloader"],
  [["2341:8050"], SAMD, "arduino:samd:mkrfox1200", "Arduino MKR FOX 1200", "app"],
  [["2341:8053"], SAMD, "arduino:samd:mkrwan1300", "Arduino MKR WAN 1300", "app"],
  [["2341:8059"], SAMD, "arduino:samd:mkrwan1310", "Arduino MKR WAN 1310", "app"],
  [["2341:8052"], SAMD, "arduino:samd:mkrgsm1400", "Arduino MKR GSM 1400", "app"],
  [["2341:8055"], SAMD, "arduino:samd:mkrnb1500", "Arduino MKR NB 1500", "app"],
  [["2341:8056"], SAMD, "arduino:samd:mkrvidor4000", "Arduino MKR Vidor 4000", "app"],
  [["2341:004d", "2341:024d", "2341:804d", "2341:824d"], SAMD, "arduino:samd:arduino_zero_native", "Arduino Zero (native USB port)", "app"],
  [["03eb:2157", "2341:2157"], SAMD, "arduino:samd:arduino_zero_edbg", "Arduino Zero (programming port)", "app"],
  // Other official Arduino cores
  [["2341:0058"], "arduino:megaavr", "arduino:megaavr:nona4809", "Arduino Nano Every", "app"],
  [["03eb:2145"], "arduino:megaavr", "arduino:megaavr:uno2018", "Arduino Uno WiFi Rev2", "app"],
  [["2341:003d", "2a03:003d"], "arduino:sam", "arduino:sam:arduino_due_x_dbg", "Arduino Due (programming port)", "app"],
  [["2341:003e", "2a03:003e"], "arduino:sam", "arduino:sam:arduino_due_x", "Arduino Due (native USB port)", "app"],
  [["2341:0266", "2341:0366", "2341:0466"], "arduino:mbed_giga", "arduino:mbed_giga:giga", "Arduino Giga R1", "app"],
  [["2341:025b", "2341:035b", "2341:045b", "2341:055b"], "arduino:mbed_portenta", "arduino:mbed_portenta:envie_m7", "Arduino Portenta H7", "app"],
  [["2341:0068", "2341:0368"], "arduino:renesas_portenta", "arduino:renesas_portenta:portenta_c33", "Arduino Portenta C33", "app"],
  [["2341:0060"], "arduino:mbed_nicla", "arduino:mbed_nicla:nicla_sense", "Arduino Nicla Sense ME", "app"],
  [["2341:025f", "2341:035f", "2341:045f", "2341:055f"], "arduino:mbed_nicla", "arduino:mbed_nicla:nicla_vision", "Arduino Nicla Vision", "app"],
  [["2341:0065"], "arduino:mbed_nicla", "arduino:mbed_nicla:nicla_voice", "Arduino Nicla Voice", "app"],
  [["2341:0064", "2341:0164", "2341:0264", "2341:0364"], "arduino:mbed_opta", "arduino:mbed_opta:opta", "Arduino Opta", "app"],
  [["2341:0078"], "arduino:zephyr", "arduino:zephyr:unoq", "Arduino UNO Q", "app"],
  // RP2350 / RP2040 (arduino-pico). A tty means a running sketch; BOOTSEL shows a drive, not a port.
  [["2e8a:000f"], PICO, "rp2040:rp2040:rpipico2", "Raspberry Pi Pico 2 (RP2350)", "app"],
  [["2e8a:f00f"], PICO, "rp2040:rp2040:rpipico2w", "Raspberry Pi Pico 2 W (RP2350)", "app"],
  [["2e8a:100a"], PICO, "rp2040:rp2040:pimoroni_pico_plus_2", "Pimoroni Pico Plus 2 (RP2350B, 8 MB PSRAM) or Tiny2040 / Plasma 2040", "app", true],
  [["2e8a:1018"], PICO, "rp2040:rp2040:pimoroni_pga2350", "Pimoroni PGA2350 (RP2350B, 8 MB PSRAM)", "app"],
  [["239a:814d"], PICO, "rp2040:rp2040:adafruit_metro_rp2350:psram=8mb", "Adafruit Metro RP2350 (RP2350B)", "app"],
  [["239a:814f"], PICO, "rp2040:rp2040:adafruit_feather_rp2350_hstx:psram=8mb", "Adafruit Feather RP2350 HSTX", "app"],
  [["1209:a184"], PICO, "rp2040:rp2040:solderparty_rp2350_stamp_xl:psram=8mb", "Solder Party RP2350 Stamp XL (RP2350B)", "app"],
  [["2886:0058"], PICO, "rp2040:rp2040:seeed_xiao_rp2350", "Seeed XIAO RP2350", "app"],
  [["1b4f:0026"], PICO, "rp2040:rp2040:sparkfun_promicrorp2350", "SparkFun Pro Micro RP2350 (8 MB PSRAM) or an RP2040 SparkFun board", "app", true],
  [["2e8a:000a"], PICO, "rp2040:rp2040:rpipico", "Raspberry Pi Pico (RP2040)", "app"],
  [["2e8a:f00a"], PICO, "rp2040:rp2040:rpipicow", "Raspberry Pi Pico W, or a board that reuses its ID", "app", true],
  [["2e8a:0003"], PICO, "rp2040:rp2040:rpipico", "RP2040 board (BOOTSEL, or a sketch reusing that ID)", "bootloader", true],
  // Espressif built-in USB-Serial/JTAG: every C3/C6/H2/C5, the S3 by default, the P4 in hwcdc mode.
  [["303a:1001"], ESP, null, "ESP32 with built-in USB (C3, C6, S3, P4)", "app", true],
  [["303a:0002"], ESP, null, "ESP32-S2/S3/P4 with TinyUSB", "app", true],
  [["2886:0056"], ESP, "esp32:esp32:XIAO_ESP32S3:USBMode=default", "Seeed XIAO ESP32-S3 (TinyUSB)", "app"],
  [["2341:0070"], ESP, "esp32:esp32:nano_nora", "Arduino Nano ESP32", "app"],
  // nRF52840: 0x80xx is the sketch, 0x00xx the bootloader
  [["239a:8029", "239a:0029", "239a:002a"], ADA, "adafruit:nrf52:feather52840", "Adafruit Feather nRF52840 Express", "app"],
  [["239a:8087", "239a:0087", "239a:0088"], ADA, "adafruit:nrf52:feather52840sense", "Adafruit Feather nRF52840 Sense", "app"],
  [["239a:8051", "239a:0051", "239a:0052"], ADA, "adafruit:nrf52:itsybitsy52840", "Adafruit ItsyBitsy nRF52840", "app"],
  [["2886:8044", "2886:0044"], SEEED, "Seeeduino:nrf52:xiaonRF52840", "Seeed XIAO nRF52840", "app"],
  // The Sense shares its IDs with Seeed's mbed core builds of both XIAOs.
  [["2886:8045", "2886:0045", "2886:0145"], SEEED, "Seeeduino:nrf52:xiaonRF52840Sense", "Seeed XIAO nRF52840 Sense (or an mbed-core XIAO)", "app", true],
  [["2886:8064", "2886:0064"], SEEED, "Seeeduino:nrf52:xiaonRF52840Plus", "Seeed XIAO nRF52840 Plus", "app"],
  [["2886:8065", "2886:0065"], SEEED, "Seeeduino:nrf52:xiaonRF52840SensePlus", "Seeed XIAO nRF52840 Sense Plus", "app"],
  // USB-UART bridges: classic ESP32 dev boards, Nano/Uno clones, ESP8266. Always ask.
  [["1a86:7523"], null, null, "CH340 USB-serial (classic ESP32 or clone Nano/Uno)", "app", true],
  [["1a86:55d4"], null, null, "CH9102 USB-serial (classic ESP32)", "app", true],
  [["10c4:ea60"], null, null, "CP210x USB-serial (classic ESP32)", "app", true],
  [["0403:6001", "0403:6015"], null, null, "FTDI USB-serial", "app", true],
];

const USB_MAP = {};
for (const [ids, core, fqbn, name, mode, ask] of USB_ROWS) {
  for (const id of ids) USB_MAP[id] = { core, fqbn, name, mode, ask: !!ask };
}

function hex4(v) {
  const n = typeof v === "number" ? v : parseInt(String(v ?? "").replace(/^0x/i, ""), 16);
  return Number.isFinite(n) ? n.toString(16).padStart(4, "0") : null;
}

/** Look up a USB identity ("0x1FFB", "1ffb" or 8187) in the offline map. */
export function boardFromUsb(vid, pid) {
  const v = hex4(vid);
  const p = hex4(pid);
  if (!v || !p) return null;
  const hit = USB_MAP[`${v}:${p}`];
  return hit ? { ...hit } : null;
}

// ── GitHub libraries ──────────────────────────────────────────────────────

const OWNER = /^[A-Za-z0-9](?:[A-Za-z0-9-]{0,38})$/;
const REPO = /^[A-Za-z0-9._-]{1,100}$/;
const REF = /^[A-Za-z0-9._\/-]{1,100}$/;

/**
 * Accepts what students paste:
 *   owner/repo · owner/repo#v1.2.0 · github.com/owner/repo
 *   https://github.com/owner/repo(.git)(/) · …/tree/<branch-or-tag>
 * Returns { owner, repo, ref, url } with url = https://github.com/owner/repo.git[#ref],
 * or { error } with a sentence saying what to type instead.
 */
export function parseGitHubLibrary(input) {
  let s = String(input ?? "").trim();
  const example = "Type owner/repo, for example pololu/romi32u4-arduino-library.";
  if (!s) return { error: example };
  let ref = null;
  const hash = s.indexOf("#");
  if (hash >= 0) {
    ref = s.slice(hash + 1).trim();
    s = s.slice(0, hash);
  }
  s = s.replace(/^git\+/i, "").replace(/^https?:\/\//i, "").replace(/^www\./i, "");
  const firstSegment = s.split("/")[0];
  if (firstSegment.includes(".") && firstSegment.toLowerCase() !== "github.com") {
    return { error: `Only GitHub libraries can be installed here (got ${firstSegment}). ${example}` };
  }
  s = s.replace(/^github\.com\//i, "").replace(/\/+$/, "");
  const parts = s.split("/");
  if (parts.length < 2) return { error: example };
  const [owner, rawRepo, ...rest] = parts;
  if (rest.length > 0) {
    if (rest[0] === "tree" && rest.length > 1 && !ref) ref = rest.slice(1).join("/");
    else return { error: `That link points inside the repository. ${example}` };
  }
  const repo = rawRepo.replace(/\.git$/i, "");
  if (!OWNER.test(owner)) return { error: `"${owner}" is not a GitHub user name. ${example}` };
  if (!REPO.test(repo) || repo === "." || repo === "..") return { error: `"${rawRepo}" is not a repository name. ${example}` };
  if (ref !== null && (!REF.test(ref) || ref.includes(".."))) return { error: `"${ref}" is not a tag or branch name.` };
  const url = `https://github.com/${owner}/${repo}.git${ref ? `#${ref}` : ""}`;
  return { owner, repo, ref, url };
}
