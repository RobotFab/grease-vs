// snippet: sensor_special_74
// tags: sensor, i2c, load-cell
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Soil moisture sensor on A0
// prompts: moisture, sensor, declare, moistureraw, float, moisturepct
// verified: compile-only
// source: kd-generated (id 74)
// exact-include: Arduino.h

#include <Arduino.h>

#define SOIL_PIN A0
#define DRY_VAL 850
#define WET_VAL 350

int moistureRaw = 0;
float moisturePct = 0.0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 2000;  // 2 seconds

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    moistureRaw = analogRead(SOIL_PIN);
    moisturePct = map(moistureRaw, DRY_VAL, WET_VAL, 0.0, 100.0);
    moisturePct = constrain(moisturePct, 0.0, 100.0);

    Serial.print("Moisture: "); Serial.print(moisturePct); Serial.println("%");

    if (moisturePct < 20.0) {
      Serial.println("WATER");
    }
  }
}
