// snippet: romi32u4_264
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: measure wheel RPM using encoders
// prompts: romi32u4, measure, wheel, encoders, include, romi32u4
// verified: compile-only
// source: kd-generated (id 264)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Encoders encoders;
Romi32U4Motors motors;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  motors.setSpeeds(200, 200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;

    int counts = encoders.getCountsAndResetLeft();
    float rpm = (counts * 60000.0f) / (500.0f * 1440.0f);  // 1440 counts per revolution
    Serial.print("RPM: "); Serial.println(rpm);
  }
}
