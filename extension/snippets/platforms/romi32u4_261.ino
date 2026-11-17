// snippet: romi32u4_261
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: drive motors forward at speed 150 for 2 seconds then stop using millis
// prompts: romi32u4, drive, motors, forward, speed, seconds
// verified: compile-only
// source: kd-generated (id 261)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;

void setup() {
  motors.setSpeeds(150, 150);
}

void loop() {
  static unsigned long startTime = 0;
  if (startTime == 0) {
    startTime = millis();
  }
  if (millis() - startTime < 2000) {
    // Keep moving forward
  } else {
    motors.setSpeeds(0, 0);  // Stop the motors
  }
}
