// snippet: sensor_special_69
// tags: sensor, i2c, load-cell, adafruit_ina219
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_INA219
// description: Read INA219 current sensor over I2C
// prompts: ina219, current, sensor, include, adafruit_ina219, declare
// verified: compile-only
// source: kd-generated (id 69)
// exact-include: Wire.h, Adafruit_INA219.h

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
unsigned long prevMs = 0;
float busVoltage, current_mA, power_mW;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  busVoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();

  Serial.print("V: "); Serial.print(busVoltage);
  Serial.print(" mA: "); Serial.print(current_mA);
  Serial.print(" mW: "); Serial.println(power_mW);
}
