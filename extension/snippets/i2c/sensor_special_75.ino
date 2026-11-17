// snippet: sensor_special_75
// tags: sensor, i2c, load-cell, adafruit_ina219
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_INA219
// description: INA219 overcurrent protection: read current_mA every 100ms
// prompts: ina219, overcurrent, protection, current_ma, 100ms, declare
// verified: compile-only
// source: kd-generated (id 75)
// exact-include: Wire.h, Adafruit_INA219.h

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
float current_mA = 0.0;
const int MOTOR_PIN = 5;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1);
  }
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, 0);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;
    current_mA = ina219.getCurrent_mA();
    Serial.print("Current: "); Serial.println(current_mA);
    if (current_mA > 500.0) {
      digitalWrite(MOTOR_PIN, 0);
      Serial.println("OVERCURRENT");
    }
  }
}
