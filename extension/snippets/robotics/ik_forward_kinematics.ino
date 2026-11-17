// snippet: ik_forward_kinematics
// tags: kinematics, fk, forward-kinematics, dh, denavit-hartenberg, 3dof, arm, xyz
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 3-DOF forward kinematics — compute end-effector XYZ from joint angles using DH notation
// prompts: forward kinematics FK, Denavit-Hartenberg DH, compute end-effector position, joint angles to XYZ, FK robot arm
// Set L1, L2, L3 to your actual link lengths in mm.
// exact-include: math.h

#include <math.h>

// ---- CONFIGURE ----
const float L1 = 0.0f;    // base height (mm) — vertical offset of shoulder pivot
const float L2 = 130.0f;  // upper arm length (mm)
const float L3 = 110.0f;  // forearm length   (mm)
// -------------------

struct Vec3 { float x, y, z; };

// th0 = base azimuth, th1 = shoulder elevation, th2 = elbow angle (all degrees)
Vec3 forwardKinematics(float th0, float th1, float th2) {
  float t0 = th0 * PI / 180.0f;
  float t1 = th1 * PI / 180.0f;
  float t2 = th2 * PI / 180.0f;

  float r  = L2 * cos(t1) + L3 * cos(t1 + t2);
  Vec3 pos;
  pos.x = r * cos(t0);
  pos.y = r * sin(t0);
  pos.z = L1 + L2 * sin(t1) + L3 * sin(t1 + t2);
  return pos;
}

void setup() {
  Serial.begin(9600);
  // test: home position (90° base, 45° shoulder, 90° elbow)
  Vec3 p = forwardKinematics(90.0f, 45.0f, 90.0f);
  Serial.print("End-effector: X="); Serial.print(p.x, 1);
  Serial.print("  Y="); Serial.print(p.y, 1);
  Serial.print("  Z="); Serial.println(p.z, 1);
}

void loop() {
  // animate joint 1 (shoulder) from 0 to 90°
  for (float th1 = 0; th1 <= 90.0f; th1 += 5.0f) {
    Vec3 p = forwardKinematics(0.0f, th1, 90.0f);
    Serial.print("th1="); Serial.print(th1, 0);
    Serial.print("  X="); Serial.print(p.x, 1);
    Serial.print("  Z="); Serial.println(p.z, 1);
    delay(100);
  }
  delay(1000);
}
