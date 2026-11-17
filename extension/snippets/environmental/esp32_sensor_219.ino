// snippet: esp32_sensor_219
// tags: esp32, sensor, millis, adafruit_vl53l0x
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_VL53L0X
// description: ESP32: read VL53L0X ToF on I2C (SDA=21 SCL=22)
// prompts: esp32, vl53l0x, (sda=21, scl=22), declare, vl53l0x
// verified: compile-only
// source: kd-generated (id 219)
// exact-include: Wire.h, Adafruit_VL53L0X.h

#include <Wire.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X tof;
unsigned long prevMs = 0;
unsigned long lastDistMs = 0;
uint16_t lastDistance = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!tof.begin()) {
    Serial.println("VL53L0X not found");
    while (1);
  }
  tof.startRangeContinuous();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;

    if (tof.isRangeComplete()) {
      uint16_t mm = tof.readRange();
      Serial.print("distance: ");
      Serial.println(mm);
      lastDistance = mm;
      lastDistMs = now;
    }
  }

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "GET_DIST") {
      Serial.print("DIST:");
      Serial.println(lastDistance);
    }
  }
}
