// snippet: ina219_power_monitor
// tags: current, voltage, power, ina219, adafruit, i2c, monitor, battery
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit INA219
// description: Measure bus voltage, current, and power with Adafruit INA219 breakout
// prompts: INA219 power monitor, measure current voltage power, battery current sensor I2C, power consumption watt meter
// exact-include: Wire.h, Adafruit_INA219.h

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
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

  float busV    = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();
  float power   = ina219.getPower_mW();
  Serial.print("V: "); Serial.print(busV);
  Serial.print(" mA: "); Serial.print(current);
  Serial.print(" mW: "); Serial.println(power);
}
