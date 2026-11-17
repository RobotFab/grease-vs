// snippet: robot_arm_ik_478
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 3-DOF IK: arm with shoulder, elbow, and wrist
// prompts: 3-dof, shoulder, elbow, wrist, solve, end-effector
// verified: compile-only
// source: kd-generated (id 478)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE: replace with your actual link lengths ----
const float L1 = 120.0f;  // upper arm length (mm)
const float L2 = 100.0f;  // forearm length (mm)
// ----------------------------------------------------------

Servo shoulder;
Servo elbow;
Servo wrist;

// Returns true if target is reachable; writes joint angles in degrees to th1, th2
bool ik2dof(float x, float y, float& th1, float& th2) {
  float r2 = x * x + y * y;
  float cosT2 = (r2 - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);
  if (cosT2 < -1.0f || cosT2 > 1.0f) return false;  // out of reach

  float t2 = acos(cosT2);                              // elbow-up solution
  float t1 = atan2(y, x) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));

  th1 = t1 * 180.0f / PI;
  th2 = t2 * 180.0f / PI;
  return true;
}

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
  wrist.attach(11);
}

void loop() {
  // trace a small square in XY space
  float targets[][2] = {{150, 0}, {150, 50}, {100, 50}, {100, 0}};

  for (auto& pt : targets) {
    float th1, th2;
    if (ik2dof(pt[0], pt[1], th1, th2)) {
      shoulder.write(constrain((int)th1, 0, 180));
      elbow.write(constrain((int)th2, 0, 180));
      float th3 = atan2(pt[1], pt[0]) * 180.0f / PI - th1 - th2;
      wrist.write(constrain((int)th3, 0, 180));
      Serial.print("X="); Serial.print(pt[0]);
      Serial.print(" Y="); Serial.print(pt[1]);
      Serial.print("  th1="); Serial.print(th1, 1);
      Serial.print("  th2="); Serial.print(th2, 1);
      Serial.print("  th3="); Serial.println(th3, 1);
    } else {
      Serial.println("Target out of reach");
    }
    delay(1000);
  }
}
