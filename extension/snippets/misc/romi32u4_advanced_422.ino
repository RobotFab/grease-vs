// snippet: romi32u4_advanced_422
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Measure and display the Romi's turning radius: drive a circle by setting left=100, right=60
// prompts: measure, display, romi's, turning, radius, drive
// verified: compile-only
// source: kd-generated (id 422)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;

const int LEFT_SPEED = 100;
const int RIGHT_SPEED = 60;
const float WHEEL_BASE_MM = 141.0f;   // distance between wheels (Romi track width)
const float COUNTS_PER_MM = 6.55f;     // Romi encoder counts per mm: 1440 CPR / (pi * 70 mm wheel)

void setup() {
  Serial.begin(9600);
  motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
}

void loop() {
  static unsigned long startTime = 0;
  static bool measured = false;

  if (startTime == 0) {
    startTime = millis();
  }

  // Drive for 2 seconds, then stop and compute turning radius
  if (!measured && millis() - startTime >= 2000) {
    motors.setSpeeds(0, 0);

    long leftCount = encoders.getCountsLeft();
    long rightCount = encoders.getCountsRight();

    // Distance each wheel travelled (mm)
    float distL = leftCount / COUNTS_PER_MM;
    float distR = rightCount / COUNTS_PER_MM;

    // Turning radius from arc geometry:
    // theta = (distL - distR) / WHEEL_BASE
    // R = (distL + distR) / (2 * theta)
    float theta = (distL - distR) / WHEEL_BASE_MM;
    float radius = 0.0f;
    if (abs(theta) > 0.001f) {  // avoid divide-by-zero
      radius = (distL + distR) / (2.0f * theta);
    }

    Serial.print("Left counts: ");  Serial.println(leftCount);
    Serial.print("Right counts: "); Serial.println(rightCount);
    Serial.print("Turning radius: ");
    Serial.print(radius);
    Serial.println(" mm");

    measured = true;
  }
}
