// snippet: robot_arm_ik_480
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: IK with joint-limit enforcement: after computing θ1 and θ2, clamp θ1 to 20°–160° and θ2 to 10°–170°
// prompts: joint-limit, enforcement, after, computing, clamp, 20°–160°
// verified: compile-only
// source: kd-generated (id 480)
// exact-include: Servo.h

#include <Servo.h>

Servo servo1, servo2;
const float L1 = 12.0;  // Length of first link in cm
const float L2 = 9.0;   // Length of second link in cm

void setup() {
  servo1.attach(9);
  servo2.attach(10);
  Serial.begin(9600);
}

void loop() {
  float x = 10.0;  // Example x-coordinate
  float y = 5.0;   // Example y-coordinate

  // 2-link IK: solve the elbow angle first, then the shoulder angle including the
  // atan2 correction term (atan2(y,x) alone is only correct when the arm is fully extended).
  float cosT2 = (x * x + y * y - L1 * L1 - L2 * L2) / (2 * L1 * L2);
  cosT2 = constrain(cosT2, -1.0f, 1.0f);
  float t2 = acos(cosT2);
  float t1 = atan2(y, x) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));

  // Convert to degrees
  float theta1 = t1 * 180.0 / PI;
  float theta2 = t2 * 180.0 / PI;

  // Clamp theta1 and theta2
  bool clamped = false;
  if (theta1 < 20.0) {
    theta1 = 20.0;
    clamped = true;
  } else if (theta1 > 160.0) {
    theta1 = 160.0;
    clamped = true;
  }
  if (theta2 < 10.0) {
    theta2 = 10.0;
    clamped = true;
  } else if (theta2 > 170.0) {
    theta2 = 170.0;
    clamped = true;
  }

  // Drive servos
  servo1.write(theta1);
  servo2.write(theta2);

  // Print clamped status
  if (clamped) {
    Serial.println("CLAMPED");
  }

  delay(1000);  // Wait for 1 second
}
