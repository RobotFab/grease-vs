// snippet: active_mode_107
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: DRIVE <left_speed> <right_speed> controls two DC motors
// prompts: active, sketch, drive, <left_speed>, <right_speed>, controls
// verified: compile-only
// source: kd-generated (id 107)
// exact-include: Arduino.h

#include <Arduino.h>

// Global variables
int leftSpeed = 0;
int rightSpeed = 0;

// Motor A: IN1=7, IN2=8, ENA=9 (PWM)
// Motor B: IN3=5, IN4=4, ENB=6 (PWM)
const int IN1 = 7, IN2 = 8, ENA = 9;
const int IN3 = 5, IN4 = 4, ENB = 6;

void motorA(int speed) {
  // speed: -255 to 255
  if (speed > 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    analogWrite(ENA, -speed);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void motorB(int speed) {
  if (speed > 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  } else if (speed < 0) {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    analogWrite(ENB, -speed);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}

void setup() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    leftSpeed = Serial.parseInt();
    rightSpeed = Serial.parseInt();
  }
  motorA(leftSpeed);
  motorB(rightSpeed);
}
