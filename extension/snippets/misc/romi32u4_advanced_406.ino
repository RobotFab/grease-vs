// snippet: romi32u4_advanced_406
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Drive the Romi straight at speed 100 for 2 seconds using millis(), stop for 1 second, then revers…
// prompts: drive, straight, speed, seconds, millis(), second
// verified: compile-only
// source: kd-generated (id 406)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
unsigned long prevMs = 0;
const unsigned long STRAIGHT_TIME = 2000;  // 2 seconds
const unsigned long STOP_TIME = 1000;     // 1 second
const unsigned long REVERSE_TIME = 2000; // 2 seconds

enum Phase { PHASE_STRAIGHT, PHASE_STOP1, PHASE_REVERSE, PHASE_DONE };
Phase phase = PHASE_STRAIGHT;

void setup() {
  motors.setSpeeds(100, 100);  // start driving straight immediately
}

void loop() {
  unsigned long now = millis();
  // Duration of the phase currently RUNNING (action was set when the phase was entered)
  unsigned long dur = (phase == PHASE_STRAIGHT) ? STRAIGHT_TIME
                   : (phase == PHASE_STOP1) ? STOP_TIME
                   : (phase == PHASE_REVERSE) ? REVERSE_TIME : 0;

  if (phase == PHASE_DONE) return;

  if (now - prevMs >= dur) {
    prevMs = now;
    switch (phase) {
      case PHASE_STRAIGHT: motors.setSpeeds(0, 0);       phase = PHASE_STOP1;   break;  // straight done -> stop
      case PHASE_STOP1:    motors.setSpeeds(-100, -100); phase = PHASE_REVERSE; break;  // stop done -> reverse
      case PHASE_REVERSE:  motors.setSpeeds(0, 0);       phase = PHASE_DONE;    break;  // reverse done -> stay stopped
      case PHASE_DONE:     break;
    }
  }
}
