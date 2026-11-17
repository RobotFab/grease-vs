// snippet: romi32u4_262
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: drive straight using encoder feedback
// prompts: romi32u4, drive, straight, encoder, feedback, include
// verified: compile-only
// source: kd-generated (id 262)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  if (millis() - prevMs >= 200) {
    prevMs = millis();

    int16_t left  = encoders.getCountsLeft();
    int16_t right = encoders.getCountsRight();

    Serial.print("L="); Serial.print(left);
    Serial.print(" R="); Serial.println(right);

    int error = left - right;
    int correction = error * 0.5;  // Adjust the gain as needed

    motors.setSpeeds(150 - correction, 150 + correction);
  }
}
