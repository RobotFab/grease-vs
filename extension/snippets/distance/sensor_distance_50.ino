// snippet: sensor_distance_50
// tags: distance, hcsr04, ultrasonic, trigger, echo
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: HC-SR04 distance alarm on pins 12/13
// prompts: hc-sr04, distance, alarm, 12/13, declare, float
// verified: compile-only
// source: kd-generated (id 50)
// exact-include: Arduino.h

#include <Arduino.h>

const int TRIG_PIN = 12;
const int ECHO_PIN = 13;
float dist = 0.0;
unsigned long trigTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - trigTime >= 200) {
    trigTime = now;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // echo pulse width in us
    if (duration > 0) {
      dist = duration * 0.034 / 2.0;
    }
    if (dist < 10.0) Serial.println("CLOSE");
    else if (dist > 50.0) Serial.println("FAR");
    else { Serial.print("distance: "); Serial.print(dist); Serial.println(" cm"); }
  }
}
