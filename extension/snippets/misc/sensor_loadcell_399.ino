// snippet: sensor_loadcell_399
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Implement a simple postal scale: read HX711 (DOUT=3, CLK=4), classify weight into postage bands (…
// prompts: implement, simple, postal, scale, hx711, (dout=3
// verified: compile-only
// source: kd-generated (id 399)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 500;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    if (scale.is_ready()) {
      float grams = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
      String band;

      if (grams >= 0 && grams <= 50) {
        band = "0-50g";
      } else if (grams > 50 && grams <= 100) {
        band = "51-100g";
      } else if (grams > 100 && grams <= 500) {
        band = "101-500g";
      } else {
        band = ">500g";
      }

      Serial.print("weight band: ");
      Serial.println(band);
    }
  }
}
