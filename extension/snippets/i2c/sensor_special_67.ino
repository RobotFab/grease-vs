// snippet: sensor_special_67
// tags: sensor, i2c, load-cell, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Read HX711 load cell on DT=4, SCK=5
// prompts: hx711, sck=5, include, hx711, declare, hx711
// verified: compile-only
// source: kd-generated (id 67)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 4
#define CLK_PIN  5

HX711 scale;
float weight = 0.0;
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
  if (now - prevMs < 200) return;
  prevMs = now;

  if (scale.is_ready()) {
    weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("weight: ");
    Serial.println(weight);
  }
}
