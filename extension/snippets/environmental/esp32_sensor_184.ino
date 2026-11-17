// snippet: esp32_sensor_184
// tags: esp32, sensor, millis, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: ESP32: read HX711 load cell on DT=4 SCK=5
// prompts: esp32, hx711, sck=5, declare, hx711, scale
// verified: compile-only
// source: kd-generated (id 184)
// exact-include: HX711.h

#include <HX711.h>

HX711 scale;
float weight = 0.0;
bool shouldTare = false;

void setup() {
  Serial.begin(9600);
  scale.begin(4, 5);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;
    if (shouldTare) {
      scale.tare();  // HX711: zero the scale (averaged reading -> offset)
      shouldTare = false;
    }
    weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("W:");
    Serial.println(weight);
  }

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "WEIGH") {
      Serial.print("W:");
      Serial.println(weight);
    } else if (command == "TARE") {
      shouldTare = true;
    }
  }
}
