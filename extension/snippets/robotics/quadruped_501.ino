// snippet: quadruped_501
// tags: quadruped, robot, body, pitch, roll, tilt, platform, stewart, servo, potentiometer, imu, stabilization
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Quadruped body pitch and roll: tilt the body while keeping all four feet planted.
//   Uses law of cosines to compute servo angle from desired platform height at each corner.
//   On ESP32, use ESP32Servo.h instead of Servo.h — standard Servo.h does not work on ESP32.
//   NOT: #include <Servo.h> on ESP32 — replace with #include <ESP32Servo.h> (library manager).
//   For a full 12-servo quadruped, replace Servo objects with PCA9685 writeMicroseconds() calls.
//   map() returns long — cast to float before trig: (float)map(...) or use manual linear scale.
// prompts: quadruped body tilt, body pitch roll quadruped, tilt platform servo, quadruped body stabilization, keep feet planted tilt body, quadruped roll pitch servo, quadruped body orientation, body lean quadruped, quadruped posture control, platform tilt four servos, quadruped IMU tilt
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE ----
const float PLATFORM_R = 60.0f;   // radius to servo pivots (mm)
const float ARM_LEN    = 40.0f;   // servo arm length (mm)
const float ROD_LEN    = 100.0f;  // pushrod length   (mm)
const float HOME_H     = 110.0f;  // nominal platform height (mm)
// -------------------

Servo servos[4];
const int SERVO_PINS[4] = {9, 10, 11, 12};

// Pivot angles: the four legs sit at the platform corners (FL, FR, BR, BL), 90° apart.
// These MUST have distinct x AND y projections — if every pivot lands on x=0 (e.g. 90/270),
// the px*sin(pitch) term is zero for all legs and body PITCH would have no effect.
const float PIVOT_DEG[4] = {45.0f, 135.0f, 225.0f, 315.0f};

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
  for (int i = 0; i < 4; i++) {
    servos[i].write(legAngle(i, height, pitch, roll));
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) servos[i].attach(SERVO_PINS[i]);
  setPlatform(HOME_H, 0, 0);
  delay(1000);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 50) {  // 20Hz
    t = millis();

    float pitch = map(analogRead(A0), 0, 1023, -10, 10);  // -10 to 10 degrees
    float roll  = map(analogRead(A1), 0, 1023, -10, 10);  // -10 to 10 degrees

    setPlatform(HOME_H, pitch, roll);
  }
}
