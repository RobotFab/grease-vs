// snippet: sensor_loadcell_391
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Initialise an HX711 load cell (DOUT=3, CLK=4)
// prompts: initialise, hx711, (dout=3, clk=4), print, weight
// verified: compile-only
// source: kd-generated (id 391)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  if (scale.is_ready()) {
    float grams = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("weight: ");
    Serial.println(grams);
  }
}
