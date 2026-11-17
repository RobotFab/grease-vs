// snippet: pico_202
// tags: pico, rp2040, raspberry-pi
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: RPi Pico: HC-SR04 on trig=8, echo=9
// prompts: ultrasonic sensor Pico, HC-SR04 RP2040, distance sensor Raspberry Pi Pico, sonar Pico, measure distance Pi Pico, HC-SR04 ultrasonic RP2040
// verified: compile-only
// source: kd-generated (id 202)
// exact-include: Arduino.h

#include <Arduino.h>

float distCm = 0.0;
unsigned long trigTime = 0;
bool waiting = false;

const int TRIG_PIN = 8;
const int ECHO_PIN = 9;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();

  if (!waiting && now - trigTime >= 200) {
    trigTime = now;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    waiting = true;
  }

  if (waiting) {
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
    if (duration > 0) {
      distCm = duration / 58.0;
      waiting = false;
      Serial.print("distance: ");
      Serial.print(distCm);
      Serial.println(" cm");
    }
  }
}
