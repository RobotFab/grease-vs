// snippet: robot_arm_fk_470
// tags: robot, arm, fk, forward-kinematics, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: FK with singularity detection: for a 2-DOF arm, detect when |θ2| < 5° (nearly straight arm, Jacob…
// prompts: singularity, detection, 2-dof, detect, (nearly, straight
// verified: compile-only
// source: kd-generated (id 470)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

const float L1 = 15.0; // Length of first link in cm
const float L2 = 10.0; // Length of second link in cm
const float THETA2_SINGULARITY = 5.0; // Singularity threshold in degrees

Servo shoulder;
Servo elbow;

float lastValidTheta1 = 0.0;
float lastValidTheta2 = 0.0;

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
}

void loop() {
  float theta1 = map(analogRead(A0), 0, 1023, 0, 180);
  float theta2 = map(analogRead(A1), 0, 1023, 0, 180);

  if (abs(theta2) < THETA2_SINGULARITY) {
    Serial.println("SINGULARITY WARNING");
    shoulder.write(constrain((int)lastValidTheta1, 0, 180));
    elbow.write(constrain((int)lastValidTheta2, 0, 180));
  } else {
    lastValidTheta1 = theta1;
    lastValidTheta2 = theta2;
    shoulder.write(constrain((int)theta1, 0, 180));
    elbow.write(constrain((int)theta2, 0, 180));
  }
}
