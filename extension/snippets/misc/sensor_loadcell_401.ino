// snippet: sensor_loadcell_401
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Accumulate a running average of HX711 readings (DOUT=3, CLK=4) using an exponential moving averag…
// prompts: accumulate, running, average, hx711, readings, (dout=3
// verified: compile-only
// source: kd-generated (id 401)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
float rawWeight = 0.0f;
float smoothedWeight = 0.0f;
const float ALPHA = 0.1f;
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
  if (now - prevMs >= 250) {  // 4 Hz
    prevMs = now;

    rawWeight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    smoothedWeight = ALPHA * rawWeight + (1.0 - ALPHA) * smoothedWeight;

    Serial.print("raw: ");
    Serial.print(rawWeight);
    Serial.print("  smoothed: ");
    Serial.println(smoothedWeight);
  }
}
