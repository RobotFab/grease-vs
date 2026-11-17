// snippet: combined_132
// tags: combined, multi-sensor, millis, adafruit_ina219
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_INA219
// description: Battery monitor with INA219 over I2C
// prompts: battery, monitor, ina219, declare, adafruit_ina219, ina219
// verified: compile-only
// source: kd-generated (id 132)
// exact-include: Wire.h, Adafruit_INA219.h

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
float voltage;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1);
  }
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 5000;  // 5 seconds

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    voltage = ina219.getBusVoltage_V();

    if (voltage < 3.3) {
      Serial.println("CRITICAL");
    } else if (voltage < 3.7) {
      Serial.println("LOW");
    } else {
      Serial.println("OK");
    }
  }
}
