// snippet: sensor_distance_51
// tags: distance, hcsr04, ultrasonic, trigger, echo
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: HC-SR04 on pins 7/8
// prompts: hc-sr04, print, distance, running, minimum, running
// verified: compile-only
// source: kd-generated (id 51)
// exact-include: Arduino.h

#include <Arduino.h>

const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
unsigned long prevMs = 0;
float minDist = 9999.0;
float maxDist = 0.0;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

long readCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
  return duration / 58;
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  long cm = readCm();
  if (cm > 0) {
    count++;
    if (cm < minDist) minDist = cm;
    if (cm > maxDist) maxDist = cm;
    Serial.print("distance: ");
    Serial.print(cm);
    Serial.print(" cm, min: ");
    Serial.print(minDist);
    Serial.print(" cm, max: ");
    Serial.print(maxDist);
    Serial.print(" cm, count: ");
    Serial.println(count);
  }
}
