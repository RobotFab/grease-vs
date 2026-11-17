// snippet: robot_arm_ik_471
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 2-DOF planar IK: receive target x,y over Serial (e
// prompts: 2-dof, planar, receive, target, compute, cosines
// verified: compile-only
// source: kd-generated (id 471)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

Servo shoulder;
Servo elbow;

const float L1 = 15.0f;  // upper arm length (cm)
const float L2 = 10.0f;  // forearm length (cm)

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    float x = input.substring(0, input.indexOf(' ')).toFloat();
    float y = input.substring(input.indexOf(' ') + 1).toFloat();

    float th1, th2;
    if (ik2dof(x, y, th1, th2)) {
      shoulder.write(constrain((int)th1, 0, 180));
      elbow.write(constrain((int)th2, 0, 180));
      Serial.print("X="); Serial.print(x);
      Serial.print(" Y="); Serial.print(y);
      Serial.print("  th1="); Serial.print(th1, 1);
      Serial.print("  th2="); Serial.println(th2, 1);
    } else {
      Serial.println("Target out of reach");
    }
  }
}

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
