// snippet: hx711_load_cell
// tags: weight, hx711, load cell, force, scale, mass, grams, kg, strain gauge
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Read weight from load cell with HX711 ADC amplifier
// prompts: load cell weight sensor, HX711 scale, measure weight grams kg, strain gauge amplifier, digital scale Arduino
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  2

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
