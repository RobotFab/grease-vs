// snippet: misc_149
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Wire
// prompts: requestfrom(), explicit, pattern, requestfrom(0x68, bytes, combine
// verified: compile-only
// source: kd-generated (id 149)
// exact-include: Wire.h

#include <Wire.h>

int16_t ax, ay, az;

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 100) {  // Read every 100ms
    t = millis();
    readAccelData();
    Serial.print("ax: "); Serial.println(ax);
    Serial.print("ay: "); Serial.println(ay);
    Serial.print("az: "); Serial.println(az);
  }
}

void readAccelData() {
  Wire.beginTransmission(0x68);  // MPU6050 address
  Wire.write(0x3B);              // Start address of accelerometer data
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6);     // Request 6 bytes

  if (Wire.available() == 6) {
    ax = (Wire.read() << 8) | Wire.read();
    ay = (Wire.read() << 8) | Wire.read();
    az = (Wire.read() << 8) | Wire.read();
  }
}
