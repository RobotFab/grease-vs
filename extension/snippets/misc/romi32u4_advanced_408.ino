// snippet: romi32u4_advanced_408
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Drive the Romi straight using encoder feedback: if right count exceeds left count, reduce right s…
// prompts: drive, straight, encoder, feedback, right, count
// verified: compile-only
// source: kd-generated (id 408)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;

const int BASE_SPEED = 80;
const unsigned long INTERVAL = 50;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    int16_t left  = encoders.getCountsLeft();
    int16_t right = encoders.getCountsRight();

    int error = left - right;
    int correction = error * 0.5;  // Adjust the gain as needed

    motors.setSpeeds(BASE_SPEED - correction, BASE_SPEED + correction);

    Serial.print("L="); Serial.print(left);
    Serial.print(" R="); Serial.print(right);
    Serial.print(" err="); Serial.println(error);
  }
}
