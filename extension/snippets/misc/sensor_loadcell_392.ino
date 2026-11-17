// snippet: sensor_loadcell_392
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Tare the HX711 (DOUT=3, CLK=4) each time a button on pin 8 (INPUT_PULLUP) is pressed
// prompts: hx711, (dout=3, clk=4), button, (input_pullup), pressed
// verified: compile-only
// source: kd-generated (id 392)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4
#define TARE_BTN 8

HX711 scale;
unsigned long prevMs = 0;
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
  const unsigned long PRINT_INTERVAL = 500;

  unsigned long now = millis();

  if (now - printMs >= PRINT_INTERVAL) {
    printMs = now;
    if (scale.is_ready()) {
      float grams = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
      Serial.print("weight: ");
      Serial.println(grams);
    }
  }

  if (digitalRead(TARE_BTN) == LOW) {
    tarePending = true;
  }

  if (tarePending) {
    scale.tare();  // HX711: zero the scale (averaged reading -> offset)
    tarePending = false;
    Serial.println("Tared");
  }
}
