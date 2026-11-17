// snippet: decl_practice_243
// tags: globals, declare, millis, nonblocking, adafruit_ina219
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_INA219
// description: Declare Adafruit_INA219 ina219 at global scope
// prompts: declare, adafruit_ina219, ina219, scope, declare, float
// verified: compile-only
// source: kd-generated (id 243)
// exact-include: Wire.h, Adafruit_INA219.h

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
float busvoltage = 0.0, current_mA = 0.0, power_mW = 0.0;
unsigned long prevMs = 0;

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

  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();

  Serial.print("V: "); Serial.println(busvoltage);
  Serial.print("mA: "); Serial.println(current_mA);
  Serial.print("mW: "); Serial.println(power_mW);
}
