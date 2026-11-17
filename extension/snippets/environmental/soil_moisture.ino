// snippet: soil_moisture
// tags: soil, moisture, analog, sensor, plant, capacitive, resistive
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read a resistive or capacitive soil moisture sensor on an analog pin.
//   Wiring: sensor signal → SOIL_PIN, VCC → 3.3V or 5V (check sensor spec), GND → GND.
//   Calibrate DRY_VAL / WET_VAL by reading the sensor in dry air and submerged in water.
// prompts: soil moisture sensor, plant watering sensor, capacitive soil sensor, measure soil humidity, garden moisture monitoring
//
// ---- CONFIGURE ----
#define SOIL_PIN  A0
#define DRY_VAL   850   // raw ADC when sensor is dry in air
#define WET_VAL   350   // raw ADC when sensor is fully wet
// -------------------

void setup() {
  Serial.begin(115200);
}

void loop() {
  int raw = analogRead(SOIL_PIN);
  int pct = map(raw, DRY_VAL, WET_VAL, 0, 100);
  pct = constrain(pct, 0, 100);

  Serial.print("Soil raw: "); Serial.print(raw);
  Serial.print("  moisture: "); Serial.print(pct); Serial.println("%");

  if (pct < 30) Serial.println("  >> Needs watering!");

  delay(1000);
}
