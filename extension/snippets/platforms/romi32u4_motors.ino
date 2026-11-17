// snippet: romi32u4_motors
// tags: romi, pololu, romi32u4, motor, encoder, robot, 32u4, drive
// boards: romi32u4
// library: Romi32U4
// description: Drive Pololu Romi 32U4 motors with encoder feedback
// prompts: Romi 32U4 motors, Pololu Romi drive, Romi encoder feedback, Romi32U4 motor control, Pololu Romi robot
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  int16_t left  = encoders.getCountsLeft();
  int16_t right = encoders.getCountsRight();

  Serial.print("L="); Serial.print(left);
  Serial.print(" R="); Serial.println(right);

  // Drive forward at half speed
  motors.setSpeeds(100, 100);
}
