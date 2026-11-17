// snippet: robot_arm_ik_479
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: IK circular trajectory: move the end-effector in a circle of radius 5 cm centred at (8, 5) cm
// prompts: circular, trajectory, end-effector, circle, radius, centred
// verified: compile-only
// source: kd-generated (id 479)
// exact-include: Servo.h

#include <Servo.h>

Servo servo1;
Servo servo2;

const float L1 = 12.0;  // upper arm length (cm)
const float L2 = 9.0;   // forearm length (cm)
const float R = 5.0;  // radius in cm
const float X_C = 8.0;  // center X in cm
const float Y_C = 5.0;  // center Y in cm
const int NUM_WAYPOINTS = 36;
const unsigned long STEP_MS = 50;

void setup() {
  servo1.attach(9);
  servo2.attach(10);
  Serial.begin(9600);
}

void loop() {
  static unsigned long t = 0;
  static int waypoint = 0;

  if (millis() - t >= STEP_MS) {
    t = millis();

    // Calculate angle for current waypoint
    float angle = (2 * PI * waypoint) / NUM_WAYPOINTS;

    // Calculate Cartesian coordinates
    float X = X_C + R * cos(angle);
    float Y = Y_C + R * sin(angle);

    // Real 2-link IK (law of cosines) for the shoulder/elbow angles at this point.
    float cosT2 = (X * X + Y * Y - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);
    cosT2 = constrain(cosT2, -1.0f, 1.0f);
    float theta2 = acos(cosT2);
    float theta1 = atan2(Y, X) - atan2(L2 * sin(theta2), L1 + L2 * cos(theta2));

    // Convert radians to degrees and clamp to the servo range.
    int servo1Angle = constrain((int)(theta1 * 180.0 / PI), 0, 180);
    int servo2Angle = constrain((int)(theta2 * 180.0 / PI), 0, 180);

    // Set servo angles
    servo1.write(servo1Angle);
    servo2.write(servo2Angle);

    // Move to next waypoint
    waypoint = (waypoint + 1) % NUM_WAYPOINTS;
  }
}
