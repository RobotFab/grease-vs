// snippet: romi32u4_pid_velocity
// tags: romi, pololu, romi32u4, a-star32u4, romi-motors, romi-encoders, counts-per-second
// note: BOARD-LOCKED — the Romi32U4 library only builds on pololu-a-star:avr:a-star32U4.
//   Do NOT use this for a generic Uno/Nano/Mega motor task (use l298n_dc_motor + pid_controller).
// boards: romi32u4
// library: Romi32U4
// description: PID velocity control on Romi32U4 — maintain a target encoder count rate (counts/s)
//   on each wheel independently. Measures delta counts over a 20 ms period, converts to counts/s,
//   low-passes the measured speed, and applies a PI controller. The derivative term is intentionally
//   omitted: at a 20 ms dt it divides encoder noise by 0.02 and slams the output between +/-400 every
//   sample, causing the limit-cycle jitter seen with the original KP/KI/KD tuning. Integral is clamped
//   (anti-windup) so it cannot saturate the output on its own.
// prompts: Romi velocity PID, Romi32U4 speed control PID, encoder velocity control Romi, counts per second PID, Romi maintain wheel speed, Romi PID motor speed
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors   motors;
Romi32U4Encoders encoders;

const float TARGET_CPS = 200.0f;    // target counts per second per wheel
const float KP = 0.20f, KI = 0.05f; // PI only — D removed (noisy at 20 ms dt)
const long  PERIOD_MS  = 20;
const float ALPHA = 0.4f;            // speed low-pass: new = ALPHA*measured + (1-ALPHA)*old

float integralL = 0, integralR = 0;
float filtL = 0, filtR = 0;
int   outputL = 0, outputR = 0;

unsigned long prevMs = 0;

// PI controller with integral anti-windup. Returns a motor command in [-400, 400].
int piControl(float error, float &integral) {
  integral += error * (PERIOD_MS / 1000.0f);
  float iTerm = KI * integral;
  if (iTerm >  380.0f) { integral =  380.0f / KI; iTerm =  380.0f; }
  if (iTerm < -380.0f) { integral = -380.0f / KI; iTerm = -380.0f; }
  float out = KP * error + iTerm;
  return constrain((int)out, -400, 400);
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

  float measL = dL * (1000.0f / PERIOD_MS);
  float measR = dR * (1000.0f / PERIOD_MS);

  // low-pass the noisy measured speed before closing the loop
  filtL = ALPHA * measL + (1.0f - ALPHA) * filtL;
  filtR = ALPHA * measR + (1.0f - ALPHA) * filtR;

  outputL = piControl(TARGET_CPS - filtL, integralL);
  outputR = piControl(TARGET_CPS - filtR, integralR);
  motors.setSpeeds(outputL, outputR);

  Serial.print("L: "); Serial.print(filtL, 0);
  Serial.print("/"); Serial.print(TARGET_CPS, 2);
  Serial.print("  R: "); Serial.print(filtR, 0);
  Serial.print("/"); Serial.println(TARGET_CPS, 2);
}
