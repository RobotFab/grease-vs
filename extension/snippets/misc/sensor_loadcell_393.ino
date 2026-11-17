// snippet: sensor_loadcell_393
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Average 10 HX711 readings (DOUT=3, CLK=4) per measurement to reduce noise
// prompts: average, hx711, readings, (dout=3, clk=4), measurement
// verified: compile-only
// source: kd-generated (id 393)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
float sum = 0.0;
int count = 0;
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
  if (now - prevMs < 500) return;
  prevMs = now;

  for (int i = 0; i < 10; i++) {
    if (scale.is_ready()) {
      sum += scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
      count++;
    }
  }

  if (count > 0) {
    float averageWeight = sum / count;
    Serial.print("weight: ");
    Serial.println(averageWeight);
    sum = 0.0;
    count = 0;
  }
}
