// snippet: sgp30_co2_voc
// tags: environmental, sgp30, co2, voc, tvoc, air-quality, i2c, adafruit, gas
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit SGP30
// description: Measure CO2 equivalent and TVOC with SGP30 — requires 15 min warm-up for accuracy
// prompts: SGP30 VOC sensor, TVOC eCO2 air quality, volatile organic compounds, indoor air quality gas, SGP30 Adafruit
// exact-include: Wire.h, Adafruit_SGP30.h

#include <Wire.h>
#include <Adafruit_SGP30.h>

Adafruit_SGP30 sgp;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!sgp.begin()) { Serial.println("SGP30 not found"); while (1); }
  Serial.print("SGP30 serial: ");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  if (!sgp.IAQmeasure()) { Serial.println("SGP30 measure failed"); return; }
  Serial.print("eCO2: "); Serial.print(sgp.eCO2); Serial.print(" ppm  ");
  Serial.print("TVOC: "); Serial.print(sgp.TVOC); Serial.println(" ppb");
}
