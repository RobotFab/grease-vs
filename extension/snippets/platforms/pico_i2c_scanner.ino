// snippet: pico_i2c_scanner
// tags: pico, rp2040, raspberry-pi, i2c, scanner, wire, address, detect, bus-scan, setSDA, setSCL, GP4, GP5
// boards: rp2040
// library: none
// description: RPi Pico: I2C bus scanner using Wire on GP4=SDA, GP5=SCL. Prints found addresses.
//   On RP2040, Wire pin assignment MUST be done via Wire.setSDA()/Wire.setSCL() before Wire.begin().
//   NOT: Wire.begin(SDA, SCL) — that is the ESP32 signature; on RP2040 it is Wire.begin() with no args.
//   NOT: Wire1 without SPI1 pin config — Wire1 uses different default pins (GP6/GP7 on some cores).
//   I2C scan technique: Wire.beginTransmission(addr) + Wire.endTransmission() returns 0 on ACK.
//   Valid 7-bit address range: 0x08–0x77 (0x00–0x07 and 0x78–0x7F are reserved).
// prompts: I2C scanner Pico, scan I2C bus RP2040, find I2C address Raspberry Pi Pico, Wire setSDA Pico, I2C detect RP2040, GP4 GP5 I2C Pico, Wire begin Pico, I2C address scan Pico, detect sensor Pico I2C
// not-tags: esp32, avr, Wire.begin(sda,scl)
// exact-include: Wire.h

#include <Wire.h>

int foundCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);  // wait for USB serial on Pico (CDC — not always instant)

  // RP2040: set I2C pins before Wire.begin(); default GP4/GP5 shown explicitly for clarity
  Wire.setSDA(4);  // GP4 = SDA
  Wire.setSCL(5);  // GP5 = SCL
  Wire.begin();    // no address args — Wire.begin(addr) makes it a slave, not what we want here

  Serial.println("Scanning I2C bus...");

  for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();  // 0 = device ACKed; 2 = NACK (no device)
    if (err == 0) {
      Serial.print("Device at 0x");
      if (addr < 0x10) Serial.print("0");
      Serial.println(addr, HEX);
      foundCount++;
    }
  }

  if (foundCount == 0) {
    Serial.println("No I2C devices found — check wiring and pull-ups");
  } else {
    Serial.print("Found ");
    Serial.print(foundCount);
    Serial.println(" device(s).");
  }
}

void loop() {}  // scan runs once in setup
