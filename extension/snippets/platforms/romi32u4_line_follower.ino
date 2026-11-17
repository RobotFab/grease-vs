// snippet: romi32u4_line_follower
// tags: romi, pololu, romi32u4, line, follower, reflectance, analog, sensor, motor
// boards: romi32u4
// library: Romi32U4
// description: Two-sensor line follower on Romi32U4 using analog reflectance on A0 (left) and
//   A2 (right). Low reading = dark (line). A proportional controller steers toward the line by
//   speeding up the wheel on the side away from the line and slowing the wheel on the line side,
//   so the robot tracks the line smoothly instead of bang-banging.
//   NOT: Romi32U4Reflectance / Romi32U4LineSensors — no such classes. Use analogRead(A0)/analogRead(A2)
//   directly for analog IR reflectance modules wired to those pins.
// prompts: Romi line follower, Romi reflectance sensor, Romi32U4 follow line, analog reflectance line tracking, Romi A0 A2 line sensor, Romi32U4 line following
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;

const int   BASE_SPEED = 150;   // forward crawl speed
const float GAIN       = 0.25f; // proportional steering gain (tune to track tightness)

void setup() {
  Serial.begin(9600);
}

void loop() {
  int leftVal  = analogRead(A0);
  int rightVal = analogRead(A2);

  // error > 0 when the line is to the RIGHT (right sensor darker), < 0 when to the LEFT.
  int error = rightVal - leftVal;

  // Romi32U4Motors clamps to ±300 by default (allowTurbo(true) raises it to ±400).
  // Constraining to ±400 here would silently clip inside the library instead.
  int leftSpeed  = constrain(BASE_SPEED + (int)(GAIN * error), -300, 300);
  int rightSpeed = constrain(BASE_SPEED - (int)(GAIN * error), -300, 300);
  motors.setSpeeds(leftSpeed, rightSpeed);

  Serial.print("L="); Serial.print(leftVal);
  Serial.print(" R="); Serial.println(rightVal);
  delay(10);
}
