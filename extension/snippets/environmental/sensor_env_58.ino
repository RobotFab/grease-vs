// snippet: sensor_env_58
// tags: sensor, i2c, temperature, pressure, humidity
// boards: avr, megaavr, romi32u4
// library: none
// description: Read MQ-2 gas sensor on A0
// prompts: sensor, declare, float, rawadc, globally, compute
// verified: compile-only
// source: kd-generated (id 58)
// exact-include: Arduino.h

#include <Arduino.h>

const int MQ2_PIN = A0;
float ppm = 0.0;
int rawADC = 0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  rawADC = analogRead(MQ2_PIN);
  ppm = 0.08 * rawADC;  // Approximate PPM calculation using ratio formula

  Serial.print("Raw ADC: "); Serial.println(rawADC);
  Serial.print("PPM: "); Serial.println(ppm);
}
