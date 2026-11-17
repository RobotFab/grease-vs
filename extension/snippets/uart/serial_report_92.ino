// snippet: serial_report_92
// tags: serial, report, uart, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: On WEIGHT command, read HX711 on DT=4 SCK=5, reply W:<grams>
// prompts: weight, command, hx711, sck=5, <grams>, scale
// verified: compile-only
// source: kd-generated (id 92)
// exact-include: HX711.h

#include <HX711.h>

HX711 scale;
float weight = 0.0;
const int DT_PIN = 4;
const int CLK_PIN = 5;

void setup() {
  Serial.begin(9600);
  scale.begin(DT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t < 100) return;
  t = millis();

  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "WEIGHT") {
      weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
      Serial.print("W:");
      Serial.println(weight);
    } else if (cmd == "TARE") {
      scale.tare();  // HX711: zero the scale (averaged reading -> offset)
      Serial.println("TARED");
    }
  }
}
