// snippet: sensor_distance_49
// tags: distance, hcsr04, ultrasonic, trigger, echo
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// description: HC-SR04 on trig=12, echo=13, triggered on a 100 ms millis() interval.
//   The INTERVAL is non-blocking; the reading itself is not. pulseIn blocks for up to its timeout
//   (30 ms here) every time it is called, so loop() stalls for that long on every miss. There is a
//   fully non-blocking alternative — attach an interrupt to the echo pin and time the edges — but
//   pulseIn is what most sketches use and its cost should be stated, not hidden.
//   A timed-out pulseIn returns 0, which means NO ECHO, not zero distance. Do not carry the last
//   good value forward silently: a robot that keeps printing 45 cm while the sensor has stopped
//   answering is reporting a measurement it did not make. Flag the miss.
//   NOT a Romi 32U4 sketch: pin 13 is the Romi's yellow LED.
// prompts: hc-sr04 trig 12 echo 13, ultrasonic every 100ms, hc-sr04 millis interval, ultrasonic distance timeout, sonar no echo handling
// verified: compile-only
// source: kd-generated (id 49)
// exact-include: Arduino.h
// not-tags: romi32u4, romi

#include <Arduino.h>

const int TRIG_PIN = 12;
const int ECHO_PIN = 13;
float distanceCm = 0.0;
bool  echoValid  = false;   // false until a ping actually comes back — check this before steering
unsigned long trigTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - trigTime >= 100) {
    trigTime = now;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // echo pulse width in us; 0 = no echo
    if (duration == 0) {
      echoValid  = false;
      distanceCm = 0.0;
      Serial.println("distance: no echo");   // out of range / absorbent / angled surface
      return;
    }
    echoValid  = true;
    distanceCm = duration * 0.034 / 2.0;     // us x 343 m/s, halved for the round trip
    Serial.print("distance: ");
    Serial.print(distanceCm, 1);
    Serial.println(" cm");
  }
}
