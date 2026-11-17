// snippet: combined_126
// tags: combined, multi-sensor, millis, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: HX711 with tare button on pin 4 (INPUT_PULLUP)
// prompts: hx711, button, (input_pullup), declare, hx711, scale
// verified: compile-only
// source: kd-generated (id 126)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  2
#define TARE_BTN 4

HX711 scale;
float weight = 0.0f;
bool tarePending = false;

void setup() {
  Serial.begin(9600);
  pinMode(TARE_BTN, INPUT_PULLUP);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  static unsigned long printMs = 0;
  static unsigned long tareMs = 0;
  const unsigned long PRINT_INTERVAL = 200;
  const unsigned long TARE_INTERVAL = 1000;

  unsigned long now = millis();

  if (now - printMs >= PRINT_INTERVAL) {
    printMs = now;
    weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("weight: ");
    Serial.println(weight);
  }

  if (now - tareMs >= TARE_INTERVAL) {
    tareMs = now;
    if (digitalRead(TARE_BTN) == LOW) {
      tarePending = true;
    }
  }

  if (tarePending) {
    scale.tare();  // HX711: zero the scale (averaged reading -> offset)
    tarePending = false;
    Serial.println("Tare complete");
  }
}
