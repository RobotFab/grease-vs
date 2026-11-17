// snippet: romi32u4_269
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: PID velocity controller using encoders
// prompts: romi32u4, velocity, controller, encoders, include, romi32u4
// verified: compile-only
// source: kd-generated (id 269)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;

const float TARGET_RPM = 60.0;
const float KP = 0.5;
const float KI = 0.1;
float integral = 0.0;

// Counts Per Revolution of the WHEEL, not the motor shaft:
//   12 counts per motor-shaft revolution × 120:1 gearbox = 1440 counts/wheel-rev.
// This is the same constant used for distance: mm_per_count = PI * 70.0 / 1440.
const float CPR = 1440.0f;

// Romi32U4Motors::setSpeeds clamps to ±300 by default (allowTurbo(true) → ±400).
// NOT 255 — that is the analogWrite() range and does not apply to this library.
const float MAX_SPEED = 300.0f;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  int16_t counts = encoders.getCountsAndResetLeft();
  // RPM = (counts / CPR) * 60 / dt_seconds
  // With dt = 0.1s (100ms): RPM = counts * 60 / (CPR * 0.1) = counts * 600 / CPR
  float rpm = (counts * 600.0f) / CPR;  // Convert counts to RPM

  float error = TARGET_RPM - rpm;
  integral += error * 0.1f;  // Integrate error over 100ms
  integral = constrain(integral, -100.0f, 100.0f);  // Anti-windup

  float output = KP * error + KI * integral;
  output = constrain(output, -MAX_SPEED, MAX_SPEED);

  motors.setSpeeds((int16_t)output, (int16_t)output);

  Serial.print("RPM: "); Serial.print(rpm);
  Serial.print("  Output: "); Serial.println(output);
}
