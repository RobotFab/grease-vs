// snippet: combined_232
// tags: combined, multi-sensor, millis, encoder
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: Encoder
// description: Motor speed governor: encoder interrupt pin 2 measures RPM
// prompts: motor, speed, governor, encoder, interrupt, measures
// verified: compile-only
// source: kd-generated (id 232)
// exact-include: Encoder.h

#include <Encoder.h>

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define MOTOR_PIN 9

Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);
volatile int encPulses = 0;
const float RPM_SCALE = 60.0 / 20.0;  // 20ms period for RPM calculation

const float TARGET_RPM = 60.0;
float measuredRPM = 0.0;
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
  if (now - prevMs < 20) return;
  prevMs = now;

  noInterrupts();
  int pulses = encPulses;
  encPulses = 0;
  interrupts();

  measuredRPM = (pulses * RPM_SCALE);
  output = pidUpdate(TARGET_RPM, measuredRPM, 0.02);
  output = constrain(output, -255, 255);
  analogWrite(MOTOR_PIN, (int)output);

  Serial.print("target: "); Serial.print(TARGET_RPM);
  Serial.print(" measured: "); Serial.print(measuredRPM);
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
