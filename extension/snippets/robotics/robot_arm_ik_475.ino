// snippet: robot_arm_ik_475
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: IK straight-line trajectory: interpolate 20 points from start (5, 0) to end (10, 8) in cm
// prompts: straight-line, trajectory, interpolate, points, start, solve
// verified: compile-only
// source: kd-generated (id 475)
// exact-include: Servo.h

#include <Servo.h>

Servo servo1;
Servo servo2;

const int L1 = 12;  // Length of first link in cm
const int L2 = 9;   // Length of second link in cm

const int NUM_POINTS = 20;
const int STEP_MS = 100;

int startX = 5;
int startY = 0;
int endX = 10;
int endY = 8;

int currentPoint = 0;
unsigned long prevMs = 0;

void setup() {
  servo1.attach(9);
  servo2.attach(10);
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= STEP_MS) {
    prevMs = now;

    if (currentPoint < NUM_POINTS) {
      float x = startX + (float)(endX - startX) * currentPoint / (NUM_POINTS - 1);
      float y = startY + (float)(endY - startY) * currentPoint / (NUM_POINTS - 1);

      // 2-link IK (law of cosines). Use float math, clamp the acos() argument, and
      // include the atan2 correction term on the shoulder angle.
      float cosT2 = (x * x + y * y - (float)L1 * L1 - (float)L2 * L2) / (2.0f * L1 * L2);
      cosT2 = constrain(cosT2, -1.0f, 1.0f);
      float t2 = acos(cosT2);
      float t1 = atan2(y, x) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));

      int theta1 = constrain((int)(t1 * 180.0 / PI), 0, 180);
      int theta2 = constrain((int)(t2 * 180.0 / PI), 0, 180);

      servo1.write(theta1);
      servo2.write(theta2);

      Serial.print("Point "); Serial.print(currentPoint); Serial.print(": ");
      Serial.print("Theta1: "); Serial.print(theta1); Serial.print(" deg, ");
      Serial.print("Theta2: "); Serial.print(theta2); Serial.println(" deg");

      currentPoint++;
    }
  }
}
