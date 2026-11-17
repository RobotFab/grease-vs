// snippet: esp32c6_serial1_romi_link
// tags: esp32, esp32c6, c6, serial1, uart, hardwareserial, pins, remap, romi, romi32u4, leonardo, level shift, voltage divider, 3.3v, 5v, ascii, bridge, gpio
// boards: esp32
// library: none
// description: The bare UART link between an ESP32-C6 and a Romi 32U4 — no BLE, no libraries.
//   Type in the C6's USB serial monitor and the characters go to the Romi; whatever the Romi
//   sends back is echoed to the monitor. Use this to prove the wiring before adding a radio.
//   FQBN esp32:esp32:esp32c6.
//
//   THE 4-ARGUMENT begin() IS THE POINT. On ESP32 the UART pins are remappable through the
//   GPIO matrix, and the 1-argument form quietly uses the core's default pins — not the ones
//   you soldered to:
//     Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
//   The order is RX then TX, and it is the C6's OWN roles: RX_PIN is the pin the C6 listens on.
//   This is the opposite of the AVR side, where Serial1 is hard-wired to pins 0 (RX) / 1 (TX)
//   and cannot be moved at all.
//
//   PIN CHOICE ON THE C6 — avoid GPIO8, GPIO9 and GPIO15 (strapping pins, sampled at reset;
//   holding one low or high changes the boot mode) and GPIO12/GPIO13 (USB Serial/JTAG, which
//   is how you are talking to the board). GPIO4 and GPIO5 are free and adjacent.
//
//   LEVEL SHIFTING — THE C6 IS NOT 5 V TOLERANT. This is the wiring that damages boards:
//     C6 TX (3.3 V) -> Romi RX pin 0     DIRECT. An AVR at 5 V has V_IH = 3.0 V, so a
//                                        3.3 V high reads correctly with 0.3 V of margin.
//     Romi TX pin 1 (5 V) -> C6 RX       NEVER DIRECT. Divide it: 1k in series, 2k to GND,
//                                        which lands 5 V at 3.33 V.
//     GND -> GND                         always, or neither side agrees what a logic level is.
//   Cross the data lines — TX to RX, RX to TX — and set the same baud on both boards.
//
//   Serial and Serial1 are different peripherals. On the C6, Serial is USB Serial/JTAG (the
//   computer) and Serial1 is a real UART (the Romi). Using both at once is the whole design.
//   NOT SoftwareSerial — the ESP32 has spare hardware UARTs and no SoftwareSerial in its core.
//   NOT I2C for this link: the Romi has to be the I2C master for its PCA9685 / AS5600 / IMU,
//     and an AVR's TWI peripheral cannot be master and slave at the same time.
// prompts: ESP32-C6 Serial1 pins, ESP32 UART to Romi, ESP32-C6 talk to Arduino Leonardo, ESP32 Serial1 begin rx tx pins, ESP32 5V level shift Arduino, ESP32-C6 voltage divider serial, ESP32 hardware serial second port, esp32c6 uart bridge romi, ESP32 GPIO strapping pins avoid, ESP32 pass through serial monitor
// not-tags: nano33, nano33iot, nano33ble, arduinoble, samd, romi32u4, avr, leonardo, softwareserial
// exact-include: none

// ── C6 side of the link ───────────────────────────────────────────────
const int ROMI_RX_PIN = 4;   // C6 listens here  <- Romi TX pin 1, THROUGH A DIVIDER
const int ROMI_TX_PIN = 5;   // C6 transmits here -> Romi RX pin 0, direct

const unsigned long LINK_BAUD = 115200;   // must match the Romi's Serial1.begin()

void setup() {
  Serial.begin(115200);                                        // USB, the computer
  Serial1.begin(LINK_BAUD, SERIAL_8N1, ROMI_RX_PIN, ROMI_TX_PIN);  // the Romi

  Serial.println("Type W A S D or space. Romi replies appear here.");
}

void loop() {
  // ── computer -> Romi ───────────────────────────────────────────────
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') continue;   // the Romi's parser wants bare characters
    Serial1.write(c);
    Serial.print("-> ");
    Serial.println(c);
  }

  // ── Romi -> computer ───────────────────────────────────────────────
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }

  // Nothing blocks here — both directions are drained every pass. No delay().
}
