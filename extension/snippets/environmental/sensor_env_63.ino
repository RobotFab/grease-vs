// snippet: sensor_env_63
// tags: sensor, i2c, temperature, pressure, humidity
// boards: avr, megaavr, romi32u4
// library: none
// description: LDR on A0: declare int rawLight = 0, float lightPct = 0
// prompts: rawlight, float, lightpct, globally, print, percentage
// verified: compile-only
// source: kd-generated (id 63)
// exact-include: Arduino.h

#include <Arduino.h>

#define LDR_PIN A0
#define DARK_THRESH 200
#define DIM_THRESH 600

int rawLight = 0;
float lightPct = 0.0;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 200) {
    prevMs = now;
    rawLight = analogRead(LDR_PIN);
    lightPct = rawLight / 1023.0f * 100.0f;

    Serial.print("Light: "); Serial.print(rawLight);
    Serial.print(" ("); Serial.print(lightPct, 1); Serial.print("%)  ");

    if (rawLight < DARK_THRESH)   Serial.println("DARK");
    else if (rawLight < DIM_THRESH) Serial.println("DIM");
    else                          Serial.println("BRIGHT");
  }
}
