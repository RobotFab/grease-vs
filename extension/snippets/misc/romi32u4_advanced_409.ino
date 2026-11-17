// snippet: romi32u4_advanced_409
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Turn the Romi approximately 90° right by driving left wheel forward and right wheel backward unti…
// prompts: approximately, right, driving, wheel, forward, right
// verified: compile-only
// source: kd-generated (id 409)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  // 90° spin in place: each wheel travels (pi/2)*(track/2) = 1.5708 * 70.5 mm ~= 111 mm.
  // At pi*70mm/1440 counts ~= 0.153 mm/count that is ~725 counts per wheel,
  // so the left-minus-right difference reaches ~1450 counts.
  static const int16_t targetDiff = 1450;
  static bool done = false;

  if (done) return;

  if (millis() - prevMs >= 10) {
    prevMs = millis();

    int16_t left  = encoders.getCountsLeft();
    int16_t right = encoders.getCountsRight();
    int16_t currentDiff = left - right;

    if (abs(currentDiff) >= targetDiff) {
      motors.setSpeeds(0, 0);  // Stop the motors
      done = true;             // latch: do not restart the turn
      Serial.println("Turn complete");
      return;
    }

    // Drive left wheel forward and right wheel backward
    motors.setSpeeds(100, -100);
  }
}
