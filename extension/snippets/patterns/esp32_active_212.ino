// snippet: esp32_active_212
// tags: esp32, active, serial, command, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: ESP32 active sketch: TARE zeros HX711 on DT=4 SCK=5
// prompts: esp32, active, sketch, zeros, hx711, sck=5
// verified: compile-only
// source: kd-generated (id 212)
// exact-include: HX711.h

#include <HX711.h>

#define DT_PIN 4
#define SCK_PIN 5

HX711 scale;
float weight = 0.0;

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(2280.0);  // Calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale tared");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "TARE") {
      scale.tare();  // HX711: zero the scale (averaged reading -> offset)
      Serial.println("Scale tared");
    } else if (command == "WEIGH") {
      weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
      Serial.print("W:");
      Serial.println(weight);
    } else if (command == "PING") {
      Serial.println("PONG");
    }
  }
}
