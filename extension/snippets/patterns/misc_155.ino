// snippet: misc_155
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Two I2C register read: Wire
// prompts: register, begintransmission(addr), write(0x3b), endtransmission(false), requestfrom(addr, int16_t
// verified: compile-only
// source: kd-generated (id 155)
// exact-include: Wire.h

#include <Wire.h>

int16_t globalVal = 0;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 1000;  // Read every second

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    int16_t val = readI2CRegister(0x3B);  // Replace 0x3B with your register address
    globalVal = val;
    Serial.print("Global Value: "); Serial.println(globalVal);
  }
}

int16_t readI2CRegister(uint8_t reg) {
  Wire.beginTransmission(0x68);  // Replace 0x68 with your device address
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, (uint8_t)2);
  return (Wire.read() << 8) | Wire.read();
}
