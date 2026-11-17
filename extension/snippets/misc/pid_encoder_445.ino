// snippet: pid_encoder_445
// tags: encoder
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: Encoder
// description: Move a motor to a sequence of positions {0, 500, 1000, 750, 200} encoder ticks
// prompts: motor, sequence, positions, encoder, ticks, (kp=2
// verified: compile-only
// source: kd-generated (id 445)
// exact-include: Encoder.h

#include <Encoder.h>

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define MOTOR_PIN 9

Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);
volatile long encoderPosition = 0;  // Use long for accumulated position

const int targetPositions[] = {0, 500, 1000, 750, 200};
const int numPositions = sizeof(targetPositions) / sizeof(targetPositions[0]);

float kp = 2.0, ki = 0.05, kd = 0.3;
float integral = 0.0, prevError = 0.0;
float output = 0.0;

unsigned long prevMs = 0;
int currentPos = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  // Non-blocking dwell after reaching a target (replaces delay(1000))
  static bool dwelling = false;
  static unsigned long dwellStart = 0;
  if (dwelling) {
    if (now - dwellStart < 1000) return;  // hold position, skip the PID update
    dwelling = false;
  }

  // Get the actual encoder position (not rate)
  noInterrupts();
  long pos = encoder.read();
  interrupts();

  // Use the absolute position directly for PID control
  // The original code incorrectly computed a rate (pulses * RPM_SCALE) and compared
  // it to absolute position targets, which would never converge
  float measuredPosition = (float)pos;
  output = pidUpdate(targetPositions[currentPos], measuredPosition, 0.02);
  output = constrain(output, -255, 255);
  analogWrite(MOTOR_PIN, (int)output);

  if (abs(measuredPosition - targetPositions[currentPos]) < 10) {
    currentPos++;
    if (currentPos >= numPositions) {
      currentPos = 0;
    }
    dwelling = true;   // dwell 1 s non-blockingly (was delay(1000))
    dwellStart = now;
  }
}

void encoderISR() {
  // Encoder library handles the counting internally
  // This ISR is just a placeholder to trigger the encoder library's ISR
  // The actual position is read via encoder.read() in loop()
}

float pidUpdate(float sp, float pv, float dt) {
  float err = sp - pv;
  integral += err * dt;
  integral = constrain(integral, -200.0, 200.0);  // anti-windup
  float deriv = (err - prevError) / dt;
  prevError = err;
  return kp * err + ki * integral + kd * deriv;
}
