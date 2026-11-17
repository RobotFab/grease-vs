// snippet: romi32u4_p_control_straight
// tags: romi, pololu, romi32u4, motor, encoder, p-control, straight, drive, correction
// boards: romi32u4
// library: Romi32U4
// description: Drive Romi32U4 straight using P-control: reset encoder deltas each cycle,
//   correct speed difference with Kp * error to keep left and right counts equal.
// prompts: Romi drive straight, Romi P-control straight, encoder correction Romi, keep encoder counts equal, Romi32U4 straight line P control
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors   motors;
Romi32U4Encoders encoders;

const int   BASE_SPEED = 100;
const float KP         = 0.5f;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  int16_t dL = encoders.getCountsAndResetLeft();
  int16_t dR = encoders.getCountsAndResetRight();

  int error      = dL - dR;           // positive = left faster
  int correction = (int)(KP * error);

  motors.setSpeeds(BASE_SPEED - correction, BASE_SPEED + correction);

  Serial.print("dL="); Serial.print(dL);
  Serial.print(" dR="); Serial.print(dR);
  Serial.print(" err="); Serial.println(error);
}
