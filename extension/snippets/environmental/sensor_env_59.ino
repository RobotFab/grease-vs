// snippet: sensor_env_59
// tags: sensor, i2c, temperature, pressure, humidity
// boards: avr, megaavr, romi32u4
// library: none
// description: Read MQ-7 CO sensor on A0
// prompts: sensor, calibrate, startup, average, samples, store
// verified: compile-only
// source: kd-generated (id 59)
// exact-include: Arduino.h

#include <Arduino.h>

const int MQ7_PIN = A0;
const int NUM_SAMPLES = 10;
float Ro = 10.0;
float Rs = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Ro = calibrateMQ7();
  Serial.print("Calibrated Ro: "); Serial.println(Ro);
}

float calibrateMQ7() {
  float sum = 0.0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(MQ7_PIN);
    delay(100);
  }
  return sum / NUM_SAMPLES;
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  Rs = readMQ7();
  float ppm = calculatePPM(Rs);
  Serial.print("PPM: "); Serial.println(ppm);
}

float readMQ7() {
  int raw = analogRead(MQ7_PIN);
  return raw;
}

float calculatePPM(float Rs) {
  float ratio = Rs / Ro;
  return 1000 * (pow(ratio, 1.186));  // PPM formula for MQ-7
}
