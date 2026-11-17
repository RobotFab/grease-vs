// snippet: active_mode_108
// tags: active, serial, command, response, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Active sketch: TARE zeros HX711 scale, replies TARED
// prompts: active, sketch, zeros, hx711, scale, replies
// verified: compile-only
// source: kd-generated (id 108)
// exact-include: HX711.h

#include <HX711.h>

// Global variables
HX711 scale;
float weight = 0.0;

void setup() {
  Serial.begin(9600);
  scale.begin(3, 2);  // DOUT, CLK
  scale.set_scale(2280.0);  // calibration factor
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("TARED");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "WEIGH") {
      weight = scale.get_units(10);  // HX711: averaged reading in units (after tare + set_scale)
      Serial.print("W:");
      Serial.println(weight);
    }
  }
}
