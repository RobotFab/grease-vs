// snippet: sensor_hall_585
// tags: encoder
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: none
// description: Closed-loop fan speed control: PID controller targeting 2000 RPM measured by Hall sensor on pin 2
// prompts: closed-loop, speed, control, controller, targeting, measured
// verified: compile-only
// source: kd-generated (id 585)
// exact-include: Arduino.h

#include <Arduino.h>

const int FAN_PIN = 5;    // PWM output for fan
const int HALL_PIN = 2;   // Hall sensor input
const int PULSES_PER_REV = 2;

const float Kp = 0.05f;  // proportional gain
const float Ki = 0.002f; // integral gain
const float Kd = 0.0f;   // derivative gain

int targetRPM = 2000;

volatile long encoderCount = 0;

// ISR: count one pulse per rising edge
void hallISR() {
  encoderCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallISR, RISING);
}

unsigned long prevMs = 0;
const unsigned long INTERVAL = 1000;  // 1 second

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  // Atomically grab and reset count
  noInterrupts();
  long count = encoderCount;
  encoderCount = 0;
  interrupts();

  // RPM = (pulses / pulses-per-rev) * (60 s / interval in seconds)
  float measuredRPM = (count * 60.0f) / PULSES_PER_REV;

  float error = targetRPM - measuredRPM;

  static float integral = 0.0f;
  static float prevError = 0.0f;

  float dt = INTERVAL / 1000.0f;  // seconds
  integral += error * dt;
  integral = constrain(integral, -200.0, 200.0);  // anti-windup
  float derivative = (error - prevError) / dt;
  prevError = error;

  float output = Kp * error + Ki * integral + Kd * derivative;
  output = constrain(output, 0.0, 255.0);

  analogWrite(FAN_PIN, (int)output);

  Serial.print("Setpoint: "); Serial.print(targetRPM);
  Serial.print(" RPM: "); Serial.println(measuredRPM);
}
