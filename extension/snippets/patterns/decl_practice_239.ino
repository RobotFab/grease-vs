// snippet: decl_practice_239
// tags: globals, declare, millis, nonblocking, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Declare HX711 scale at global scope
// prompts: declare, hx711, scale, scope, declare, float
// verified: compile-only
// source: kd-generated (id 239)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  2

HX711 scale;
float weight = 0.0;
float tare = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  tare = scale.get_units(5);  // get tare value
  Serial.println("Scale ready — place weight");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  if (scale.is_ready()) {
    weight = scale.get_units() - tare;  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("weight: ");
    Serial.println(weight);
  }
}
