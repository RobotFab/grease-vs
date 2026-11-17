// snippet: sensor_loadcell_395
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Run a guided calibration routine over Serial for the HX711 (DOUT=3, CLK=4): prompt for tare, then known weight
// prompts: guided, calibration, routine, hx711, (dout=3, clk=4)
// verified: compile-only
// source: kd-generated (id 395)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
float calibrationFactor = 0.0;
bool isTared = false;
const float KNOWN_WEIGHT = 200.0;  // grams

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT_PIN, CLK_PIN);
  Serial.println("HX711 calibration routine");
  Serial.println("1. Place the load cell on a flat surface and press 'TARE'");
  Serial.println("2. Place a known 200g weight on the load cell and press 'CAL'");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "TARE") {
      scale.tare();  // HX711: zero the scale (averaged reading -> offset)
      isTared = true;
      Serial.println("Tare complete");
    } else if (cmd == "CAL" && isTared) {
      // Read raw ADC value (averaged), then compute scale factor
      long rawValue = scale.get_value(10);  // average of 10 raw readings
      calibrationFactor = (float)rawValue / KNOWN_WEIGHT;
      scale.set_scale(calibrationFactor);  // HX711: calibration factor (units per raw count)
      Serial.print("Calibration factor: ");
      Serial.println(calibrationFactor, 2);
      Serial.print("Weight check: ");
      Serial.print(scale.get_units(5), 2);  // HX711: averaged reading in units (after tare + set_scale)
      Serial.println(" g");
      Serial.println("Calibration complete");
    } else if (cmd == "CAL" && !isTared) {
      Serial.println("Please tare before calibrating");
    }
  }
}
