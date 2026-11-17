// snippet: romi32u4_advanced_417
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Implement speed ramp: on buttonA press, ramp motors from 0 to 150 over 1 second using millis(), h…
// prompts: implement, speed, buttona, press, motors, second
// verified: compile-only
// source: kd-generated (id 417)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4ButtonA buttonA;  // Romi buttons/LEDs go through the library, not raw pin numbers

enum State { IDLE, RAMP_UP, HOLD, RAMP_DOWN };
State state = IDLE;

const int TARGET_SPEED = 150;
const unsigned long RAMP_TIME = 1000;  // 1 second up, 1 second down
const unsigned long HOLD_TIME = 2000;  // 2 seconds at full speed
unsigned long stateStart = 0;

void setup() {
  motors.setSpeeds(0, 0);
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - stateStart;

  switch (state) {
    case IDLE:
      if (buttonA.getSingleDebouncedPress()) {
        state = RAMP_UP;
        stateStart = now;
        ledYellow(true);
      }
      break;

    case RAMP_UP:
      if (elapsed >= RAMP_TIME) {
        motors.setSpeeds(TARGET_SPEED, TARGET_SPEED);
        state = HOLD;
        stateStart = now;
      } else {
        int s = (long)TARGET_SPEED * elapsed / RAMP_TIME;  // linear 0 -> TARGET over RAMP_TIME
        motors.setSpeeds(s, s);
      }
      break;

    case HOLD:
      if (elapsed >= HOLD_TIME) {
        state = RAMP_DOWN;
        stateStart = now;
      }
      break;

    case RAMP_DOWN:
      if (elapsed >= RAMP_TIME) {
        motors.setSpeeds(0, 0);
        state = IDLE;
        ledYellow(false);
      } else {
        int s = TARGET_SPEED - (long)TARGET_SPEED * elapsed / RAMP_TIME;
        motors.setSpeeds(s, s);
      }
      break;
  }
}
