// snippet: esp32_sensor_164
// tags: esp32, sensor, millis
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: ESP32: HC-SR04 on trig=12, echo=14
// prompts: esp32, hc-sr04, trig=12, echo=14, non-blocking, distance
// verified: compile-only
// source: kd-generated (id 164)
// exact-include: Arduino.h

#include <Arduino.h>

float distCm = 0.0;
unsigned long trigTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(12, OUTPUT);
  pinMode(14, INPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - trigTime >= 200) {
    trigTime = now;
    digitalWrite(12, LOW);
    delayMicroseconds(2);
    digitalWrite(12, HIGH);
    delayMicroseconds(10);
    digitalWrite(12, LOW);
    long duration = pulseIn(14, HIGH, 30000);  // echo pulse width in us
    if (duration > 0) {
      distCm = duration * 0.034 / 2.0;
    }
    Serial.print("distance: ");
    Serial.print(distCm);
    Serial.println(" cm");
  }
}
