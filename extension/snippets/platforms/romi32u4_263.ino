// snippet: romi32u4_263
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: compute distance traveled using encoders
// prompts: romi32u4, compute, distance, traveled, encoders, include
// verified: compile-only
// source: kd-generated (id 263)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Encoders encoders;
unsigned long prevMs = 0;
int totalCounts = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;

    int counts = encoders.getCountsAndResetLeft();
    totalCounts += counts;

    float distance = (totalCounts * 22.0f) / 1440.0f;  // 22cm per revolution, 1440 counts per revolution
    Serial.print("Distance: "); Serial.print(distance, 2); Serial.println(" cm");
  }
}
