// snippet: robot_arm_fk_456
// tags: robot, arm, fk, forward-kinematics, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 2-DOF planar FK: read joint angles θ1, θ2 from potentiometers on A0, A1 (0–180°)
// prompts: 2-dof, planar, joint, angles, potentiometers, (0–180°)
// verified: compile-only
// source: kd-generated (id 456)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

const float L1 = 15.0f;  // upper arm length (cm)
const float L2 = 10.0f;  // forearm length (cm)

Servo shoulder;
Servo elbow;

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 200) {  // 5 Hz
    t = millis();

    float theta1 = map(analogRead(A0), 0, 1023, 0, 180);
    float theta2 = map(analogRead(A1), 0, 1023, 0, 180);

    float x = L1 * cos(theta1 * PI / 180.0f) + L2 * cos((theta1 + theta2) * PI / 180.0f);
    float y = L1 * sin(theta1 * PI / 180.0f) + L2 * sin((theta1 + theta2) * PI / 180.0f);

    Serial.print("x="); Serial.print(x, 2);
    Serial.print("  y="); Serial.println(y, 2);
  }
}
