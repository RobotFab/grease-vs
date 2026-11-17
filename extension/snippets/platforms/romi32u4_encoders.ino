// snippet: romi32u4_encoders
// tags: romi, pololu, romi32u4, encoder, encoders, ticks, counts, wheel
// boards: romi32u4
// library: Romi32U4
// description: Read both Romi 32U4 wheel encoders and print left and right tick counts, millis-based.
//   Encoders class: Romi32U4Encoders — getCountsLeft() / getCountsRight() return int16_t.
//   getCountsAndResetLeft() / getCountsAndResetRight() read and zero in one call.
//   NOT: encoders.left() / encoders.read() — no such methods.
//   The counts are int16_t and they WRAP at +-32767. At 1440 counts per wheel revolution that is
//   about 22 revolutions, roughly 5 m of travel, and then the count jumps to a large negative
//   number. For odometry over any real distance, either accumulate into a long yourself or use
//   getCountsAndReset*() every pass and add the delta — do not treat getCountsLeft() as a
//   distance-since-boot.
// prompts: Romi read encoders, Romi32U4 encoder ticks, print wheel encoder counts Romi, Romi left right tick counts, Romi encoder counts serial, how far have the wheels turned Romi
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Encoders encoders;
unsigned long prevMs = 0;

// Wrap-safe totals. The hardware counters are int16_t; these are not.
long totalLeft  = 0;
long totalRight = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;  // 20 Hz
  prevMs = now;

  // Read-and-zero, then accumulate. Each delta is small, so the int16_t never gets near its
  // limit and the running total is good for as far as the robot can drive.
  totalLeft  += encoders.getCountsAndResetLeft();
  totalRight += encoders.getCountsAndResetRight();

  Serial.print("L: ");
  Serial.print(totalLeft);
  Serial.print("  R: ");
  Serial.println(totalRight);
}
