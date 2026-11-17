// snippet: sensor_distance_54
// tags: distance, hcsr04, ultrasonic, trigger, echo
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: HC-SR04 state machine on pins 7/8: states IDLE, TRIGGER, WAIT, MEASURE
// prompts: hc-sr04, state, machine, states, trigger, measure
// verified: compile-only
// source: kd-generated (id 54)
// exact-include: Arduino.h

#include <Arduino.h>

enum State { IDLE, TRIGGER, WAIT, MEASURE };
State state = IDLE;
unsigned long stateStart = 0;
unsigned long echoStart = 0;
float lastDist = 0;

const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();

  switch (state) {
    case IDLE:
      if (now - stateStart > 1000) {
        state = TRIGGER;
        stateStart = now;
      }
      break;

    case TRIGGER:
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
      state = WAIT;
      stateStart = now;
      break;

    case WAIT:
      if (digitalRead(ECHO_PIN) == HIGH) {
        state = MEASURE;
        echoStart = micros();
      }
      break;

    case MEASURE:
      if (digitalRead(ECHO_PIN) == LOW) {
        unsigned long duration = micros() - echoStart;
        lastDist = duration * 0.034 / 2;
        Serial.print("distance: ");
        Serial.println(lastDist);
        state = IDLE;
      }
      break;
  }
}
