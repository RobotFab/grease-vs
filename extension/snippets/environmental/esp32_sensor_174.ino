// snippet: esp32_sensor_174
// tags: esp32, sensor, millis, adafruit_ina219
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_INA219
// description: ESP32: read INA219 current sensor over I2C
// prompts: esp32, ina219, current, sensor, declare, adafruit_ina219
// verified: compile-only
// source: kd-generated (id 174)
// exact-include: Wire.h, Adafruit_INA219.h

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
float busVoltage, current_mA;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1);
  }
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;
    busVoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    Serial.print("V: "); Serial.print(busVoltage);
    Serial.print(" mA: "); Serial.print(current_mA);
    if (current_mA > 800) {
      Serial.println(" OVERCURRENT");
    } else {
      Serial.println();
    }
  }
}
