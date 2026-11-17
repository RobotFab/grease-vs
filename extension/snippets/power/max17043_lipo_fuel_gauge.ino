// snippet: max17043_lipo_fuel_gauge
// tags: power, battery, lipo, fuel-gauge, max17048, soc, voltage, i2c, adafruit
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit MAX1704X
// description: Read LiPo battery state-of-charge (%) and voltage with MAX17048 (Adafruit MAX1704X library).
//   Note: MAX17048 is register-incompatible with MAX17043/17044 — the Adafruit_MAX1704X
//   library's Adafruit_MAX17048 class targets the 17048/17049. Use the correct chip
//   for your hardware; do not mix MAX17043 code with MAX17048 library calls.
// prompts: LiPo fuel gauge, battery percentage SOC, MAX17048 state of charge, battery level I2C, LiPo voltage percent
// exact-include: Wire.h, Adafruit_MAX1704X.h

#include <Wire.h>
#include <Adafruit_MAX1704X.h>

Adafruit_MAX17048 maxlipo;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Wire.begin();
  if (!maxlipo.begin()) { Serial.println("MAX17043 not found"); while (1); }
  Serial.print("MAX17043 chip ID: "); Serial.println(maxlipo.getChipID(), HEX);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  Serial.print("Vbat: "); Serial.print(maxlipo.cellVoltage(), 3); Serial.print(" V  ");
  Serial.print("SOC: ");  Serial.print(maxlipo.cellPercent(), 1); Serial.println("%");
}
