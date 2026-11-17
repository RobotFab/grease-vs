// snippet: sensor_loadcell_397
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Detect rapid weight change on HX711 (DOUT=3, CLK=4): if delta > 50 g in 100 ms, print "IMPACT" an…
// prompts: detect, rapid, weight, change, hx711, (dout=3
// verified: compile-only
// source: kd-generated (id 397)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
float prevWeight = 0.0;
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
  if (now - prevMs < 100) return;
  prevMs = now;

  if (scale.is_ready()) {
    float currentWeight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    float weightDelta = abs(currentWeight - prevWeight);

    if (weightDelta > 50.0) {
      Serial.print("IMPACT: ");
      Serial.print(currentWeight);
      Serial.println(" g");
    }

    prevWeight = currentWeight;
  }
}
