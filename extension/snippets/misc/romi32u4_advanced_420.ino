// snippet: romi32u4_advanced_420
// tags: romi, pololu, romi32u4, pid, speed, control, encoder, motor
// boards: romi32u4
// library: Romi32U4
// description: PID speed control on both Romi 32U4 wheels. Target is set in encoder ticks per
//   100 ms; actual speed is measured from Romi32U4Encoders deltas and the error is closed with a
//   PI controller (derivative omitted — noisy on the raw speed). motors.setSpeeds() takes SIGNED
//   commands, so the sign is preserved and a wheel can slow down or reverse to correct overspeed.
//   Do NOT use attachInterrupt()/analogWrite() on guessed pins — the Romi wheel encoders and motor
//   driver are only reachable through the Romi32U4 library.
// prompts: speed, control, wheels, target, speed, ticks/100ms
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors   motors;
Romi32U4Encoders encoders;

const float TARGET_SPEED = 50.0f;    // target ticks per 100 ms per wheel
const float KP = 0.20f, KI = 0.05f;  // PI only — D removed (noisy at this dt)
const unsigned long PERIOD_MS = 100;

float integralL = 0, integralR = 0;
int   outputL = 0, outputR = 0;
unsigned long prevMs = 0;

// PI controller with integral anti-windup. Returns a signed motor command in [-400, 400].
int piControl(float error, float &integral) {
  integral += error * (PERIOD_MS / 1000.0f);
  float iTerm = KI * integral;
  if (iTerm >  380.0f) { integral =  380.0f / KI; iTerm =  380.0f; }
  if (iTerm < -380.0f) { integral = -380.0f / KI; iTerm = -380.0f; }
  return constrain((int)(KP * error + iTerm), -400, 400);
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < PERIOD_MS) return;
  prevMs = now;

  int16_t dL = encoders.getCountsAndResetLeft();
  int16_t dR = encoders.getCountsAndResetRight();

  float actualL = (float)dL;  // ticks counted in this 100 ms window = ticks/100ms
  float actualR = (float)dR;

  outputL = piControl(TARGET_SPEED - actualL, integralL);
  outputR = piControl(TARGET_SPEED - actualR, integralR);
  motors.setSpeeds(outputL, outputR);

  Serial.print("L: "); Serial.print(actualL, 2);
  Serial.print("/"); Serial.print(TARGET_SPEED, 2);
  Serial.print("  R: "); Serial.print(actualR, 2);
  Serial.print("/"); Serial.println(TARGET_SPEED, 2);
}
