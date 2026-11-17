// snippet: ik_3dof_parallel_stewart
// tags: kinematics, stewart, parallel, 3dof, platform, hexapod, servo, tilt, roll, pitch
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Simplified 3-DOF Stewart-style platform IK — 3 servos control height + pitch + roll
// prompts: Stewart platform IK, 3DOF parallel platform, hexapod platform servos, tilt platform IK, Stewart 3 servo
// Set PLATFORM_R (platform radius) and ARM_LEN to your hardware dimensions in mm.
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE ----
const float PLATFORM_R = 60.0f;   // radius to servo pivots (mm)
const float ARM_LEN    = 40.0f;   // servo arm length (mm)
const float ROD_LEN    = 100.0f;  // pushrod length   (mm)
const float HOME_H     = 110.0f;  // nominal platform height (mm)
// -------------------

Servo servos[3];
const int SERVO_PINS[3] = {9, 10, 11};

// Pivot angles around platform (120° apart)
const float PIVOT_DEG[3] = {90.0f, 210.0f, 330.0f};

// Compute servo angle for one leg given desired platform height, pitch, roll (degrees)
int legAngle(int i, float height, float pitch, float roll) {
  float px = PLATFORM_R * cos(PIVOT_DEG[i] * PI / 180.0f);
  float py = PLATFORM_R * sin(PIVOT_DEG[i] * PI / 180.0f);

  // desired z at this leg attachment point
  float pRad = pitch * PI / 180.0f;
  float rRad = roll  * PI / 180.0f;
  float z = height - px * sin(pRad) + py * sin(rRad);

  // law of cosines to find servo angle
  float h2 = z * z;
  float d2 = h2;  // simplified: horizontal offset ignored for small angles
  float cosA = (ARM_LEN * ARM_LEN + d2 - ROD_LEN * ROD_LEN) / (2.0f * ARM_LEN * sqrt(d2));
  cosA = constrain(cosA, -1.0f, 1.0f);
  float angleDeg = acos(cosA) * 180.0f / PI;
  return constrain((int)angleDeg, 0, 180);
}

void setPlatform(float height, float pitch, float roll) {
  for (int i = 0; i < 3; i++) {
    servos[i].write(legAngle(i, height, pitch, roll));
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) servos[i].attach(SERVO_PINS[i]);
  setPlatform(HOME_H, 0, 0);
  delay(1000);
}

void loop() {
  // rock forward/back then left/right
  for (float p = -15; p <= 15; p += 3) { setPlatform(HOME_H, p, 0); delay(80); }
  for (float r = -15; r <= 15; r += 3) { setPlatform(HOME_H, 0, r); delay(80); }
  setPlatform(HOME_H, 0, 0); delay(500);
}
