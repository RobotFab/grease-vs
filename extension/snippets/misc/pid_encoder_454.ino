// snippet: pid_encoder_454
// tags: encoder
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: Encoder
// description: Cascade PID: outer loop controls position (Kp=1
// prompts: cascade, outer, controls, position, (kp=1, outputs
// verified: compile-only
// source: kd-generated (id 454)
// exact-include: Encoder.h

#include <Encoder.h>

const int ENC_A = 2;
const int ENC_B = 3;
const int MOTOR_IN1 = 5;
const int MOTOR_IN2 = 6;
const int MOTOR_EN = 7;

Encoder encoder(ENC_A, ENC_B);
float targetPosition = 0.0;
float currentVelocity = 0.0;
float currentPosition = 0.0;
float lastPosition = 0.0;
float lastVelocity = 0.0;
float lastTime = 0.0;

const float KP_OUTER = 1.0;
const float KP_INNER = 3.0;
const float KI_INNER = 0.2;
const float KD_INNER = 0.01;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_EN, OUTPUT);
  lastTime = millis();
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  // Outer loop: position control
  float errorPosition = targetPosition - currentPosition;
  float velocitySetpoint = KP_OUTER * errorPosition;

  // Inner loop: velocity control
  float errorVelocity = velocitySetpoint - currentVelocity;
  float acceleration = KP_INNER * errorVelocity + KI_INNER * errorVelocity * dt + KD_INNER * (errorVelocity - lastVelocity) / dt;
  currentVelocity += acceleration * dt;

  // Update motor speed
  int motorSpeed = (int)currentVelocity;
  if (motorSpeed > 255) motorSpeed = 255;
  if (motorSpeed < -255) motorSpeed = -255;
  analogWrite(MOTOR_EN, abs(motorSpeed));
  digitalWrite(MOTOR_IN1, motorSpeed > 0);
  digitalWrite(MOTOR_IN2, motorSpeed < 0);

  // Update encoder position
  lastPosition = currentPosition;
  currentPosition = encoder.read();
  currentVelocity = (currentPosition - lastPosition) / dt;
  lastVelocity = errorVelocity;  // remember for next iteration's D term

  // Print values for debugging
  Serial.print("Position: "); Serial.println(currentPosition);
  Serial.print("Velocity: "); Serial.println(currentVelocity);
  Serial.print("Motor Speed: "); Serial.println(motorSpeed);
  Serial.println();
}
