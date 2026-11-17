// snippet: robot_arm_ik_482
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Iterative IK using gradient descent: start from current joint angles, iteratively adjust θ1,θ2 to…
// prompts: iterative, gradient, descent, start, current, joint
// verified: compile-only
// source: kd-generated (id 482)
// exact-include: Arduino.h

#include <Arduino.h>

// Define the target position
// Target must lie inside the workspace: reach = l1 + l2 = 10, so (10,10) at radius
// 14.1 is UNREACHABLE and gradient descent would never converge. (5,5) at radius 7.07 is.
const float targetX = 5.0;
const float targetY = 5.0;

// Define the initial joint angles
float theta1 = 0.0;
float theta2 = 0.0;

// Define the lengths of the links
const float l1 = 5.0;
const float l2 = 5.0;

// Define the step size and maximum iterations
const float stepSize = 0.01;
const int maxIterations = 100;

// Function to calculate the forward kinematics
void forwardKinematics(float theta1, float theta2, float &x, float &y) {
  x = l1 * cos(theta1) + l2 * cos(theta1 + theta2);
  y = l1 * sin(theta1) + l2 * sin(theta1 + theta2);
}

// Function to calculate the error
float calculateError(float x, float y) {
  return sqrt((x - targetX) * (x - targetX) + (y - targetY) * (y - targetY));
}

// Function to update the joint angles using gradient descent
void updateAngles(float &theta1, float &theta2) {
  float x, y;

  // Numerical gradient of the error via central differences. Sampling only +/-stepSize
  // avoids the division-by-zero that a zero-magnitude finite-difference step produces.
  float xP, yP, xM, yM;

  forwardKinematics(theta1 + stepSize, theta2, xP, yP);
  forwardKinematics(theta1 - stepSize, theta2, xM, yM);
  float dTheta1 = (calculateError(xP, yP) - calculateError(xM, yM)) / (2.0f * stepSize);

  forwardKinematics(theta1, theta2 + stepSize, xP, yP);
  forwardKinematics(theta1, theta2 - stepSize, xM, yM);
  float dTheta2 = (calculateError(xP, yP) - calculateError(xM, yM)) / (2.0f * stepSize);

  theta1 -= stepSize * dTheta1;
  theta2 -= stepSize * dTheta2;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 100) {
    t = millis();
    updateAngles(theta1, theta2);
    Serial.print("theta1: "); Serial.println(theta1);
    Serial.print("theta2: "); Serial.println(theta2);
  }
}
