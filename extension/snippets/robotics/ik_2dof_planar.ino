// snippet: ik_2dof_planar
// tags: kinematics, ik, inverse-kinematics, 2dof, planar, arm, servo, robot, elbow
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 2-DOF planar arm inverse kinematics — computes shoulder/elbow angles for XY target
// prompts: 2DOF planar arm IK, robot arm inverse kinematics, elbow shoulder angles, two-link arm IK servo, planar 2 DOF
// Set L1 and L2 to your actual link lengths in mm.
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE: replace with your actual link lengths ----
const float L1 = 120.0f;  // upper arm length (mm)
const float L2 = 100.0f;  // forearm length (mm)
// ----------------------------------------------------------

Servo shoulder;
Servo elbow;

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
}

void loop() {
  // trace a small square in XY space
  float targets[][2] = {{150, 0}, {150, 50}, {100, 50}, {100, 0}};

  for (auto& pt : targets) {
    float th1, th2;
    if (ik2dof(pt[0], pt[1], th1, th2)) {
      shoulder.write(constrain((int)(th1 + 90), 0, 180));
      elbow.write(constrain((int)th2, 0, 180));
      Serial.print("X="); Serial.print(pt[0]);
      Serial.print(" Y="); Serial.print(pt[1]);
      Serial.print("  th1="); Serial.print(th1, 1);
      Serial.print("  th2="); Serial.println(th2, 1);
    } else {
      Serial.println("Target out of reach");
    }
    delay(1000);
  }
}
