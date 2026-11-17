// snippet: romi32u4_advanced_410
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Implement a stop-and-go sequence: drive forward for 500 encoder ticks, stop for 1 second, turn 90…
// prompts: implement, stop-and-go, sequence, drive, forward, encoder
// verified: compile-only
// source: kd-generated (id 410)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4Encoders encoders;

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  static int16_t forwardTicks = 500;
  static int16_t turnTicks = 725;   // 90° spin ~= 725 counts per wheel (pi/2 * 70.5mm track radius / 0.153mm-per-count)
  static int16_t currentTicks = 0;
  static int16_t stopTime = 1000;  // 1 second
  static unsigned long stopStartTime = 0;
  static int16_t turnCount = 0;
  static int16_t maxTurns = 4;
  static int16_t prevLeft = 0;
  static int16_t prevRight = 0;
  static enum { FWD, STOP, TURN, DONE } state = FWD;

  if (millis() - prevMs >= 10) {
    prevMs = millis();

    int16_t left  = encoders.getCountsLeft();
    int16_t right = encoders.getCountsRight();
    int16_t dL = left - prevLeft;
    int16_t dR = right - prevRight;
    prevLeft = left;
    prevRight = right;
    // Forward motion: wheels agree, average them. Spin: wheels oppose, so the
    // average is ~0 — use the half-difference instead or the turn never finishes.
    int16_t deltaFwd  = (dL + dR) / 2;
    int16_t deltaTurn = (dL - dR) / 2;

    switch (state) {
      case FWD:
        motors.setSpeeds(100, 100);
        currentTicks += deltaFwd;
        if (currentTicks >= forwardTicks) {
          motors.setSpeeds(0, 0);
          stopStartTime = millis();
          currentTicks = 0;
          state = STOP;
        }
        break;
      case STOP:
        motors.setSpeeds(0, 0);
        if (millis() - stopStartTime >= stopTime) {
          if (turnCount < maxTurns) state = TURN;
          else state = DONE;
        }
        break;
      case TURN:
        motors.setSpeeds(100, -100);
        currentTicks += deltaTurn;
        if (currentTicks >= turnTicks) {
          motors.setSpeeds(0, 0);
          currentTicks = 0;
          turnCount++;
          state = (turnCount >= maxTurns) ? DONE : FWD;
        }
        break;
      case DONE:
        motors.setSpeeds(0, 0);
        return;
    }
  }
}
