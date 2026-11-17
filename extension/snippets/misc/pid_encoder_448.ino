// snippet: pid_encoder_448
// tags: encoder
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: Encoder
// description: Velocity PID with encoder: measure velocity as delta-ticks per 50 ms
// prompts: velocity, encoder, measure, velocity, delta-ticks, setpoint
// verified: compile-only
// source: kd-generated (id 448)
// exact-include: Encoder.h

#include <Encoder.h>

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define MOTOR_PIN 9
#define SETPOINT_PIN A0

Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);
volatile int encPulses = 0;
const float VELOCITY_SCALE = 1000.0 / 125.0;  // 125ms loop period (matches loop() gate below)

const float TARGET_VELOCITY = 0.0;
float measuredVelocity = 0.0;
float output = 0.0;

float kp = 1.0, ki = 0.01, kd = 0.1;
float integral = 0.0, prevError = 0.0;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 125) return;  // 4 Hz
  prevMs = now;

  noInterrupts();
  int pulses = encPulses;
  encPulses = 0;
  interrupts();

  measuredVelocity = (pulses * VELOCITY_SCALE);
  float setpoint = map(analogRead(SETPOINT_PIN), 0, 1023, -200, 200);
  output = pidUpdate(setpoint, measuredVelocity, 0.125);
  output = constrain(output, -255, 255);
  analogWrite(MOTOR_PIN, (int)output);

  Serial.print("setpoint: "); Serial.print(setpoint);
  Serial.print(" velocity: "); Serial.print(measuredVelocity);
  Serial.print(" output: "); Serial.println(output);
}

void encoderISR() {
  encPulses++;
}

float pidUpdate(float sp, float pv, float dt) {
  float err = sp - pv;
  integral += err * dt;
  integral = constrain(integral, -200.0, 200.0);  // anti-windup
  float deriv = (err - prevError) / dt;
  prevError = err;
  return kp * err + ki * integral + kd * deriv;
}
