// snippet: sensor_env_60
// tags: sensor, i2c, temperature, pressure, humidity
// boards: avr, megaavr, romi32u4
// library: none
// description: Read MQ-135 air quality sensor on A0
// prompts: mq-135, quality, sensor, declare, rawval, globally
// verified: compile-only
// source: kd-generated (id 60)
// exact-include: Arduino.h

#include <Arduino.h>

const int MQ135_PIN = A0;
int rawVal = 0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  rawVal = analogRead(MQ135_PIN);
  String category = getQualityCategory(rawVal);
  Serial.print("Raw Value: "); Serial.println(rawVal);
  Serial.print("Category: "); Serial.println(category);
}

String getQualityCategory(int value) {
  if (value < 200) return "GOOD";
  else if (value < 400) return "FAIR";
  else return "POOR";
}
