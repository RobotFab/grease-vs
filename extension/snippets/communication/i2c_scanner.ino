// snippet: i2c_scanner
// tags: i2c, wire, scanner, address, detect, bus, debug
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Wire
// description: Scan I2C bus and print addresses of all connected devices
// prompts: I2C scanner, scan I2C bus, find I2C address, detect I2C device, Wire scanner, I2C address discovery
// exact-include: Wire.h

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  Wire.begin();
  Serial.println("I2C Scanner");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("Device at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Scan complete");
}

void loop() {}
