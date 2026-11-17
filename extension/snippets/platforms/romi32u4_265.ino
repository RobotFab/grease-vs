// snippet: romi32u4_265
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: button A starts and stops motors
// prompts: romi32u4, button, starts, stops, motors, include
// verified: compile-only
// source: kd-generated (id 265)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4ButtonA buttonA;
Romi32U4Motors motors;
bool running = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. A=start/stop");
}

void loop() {
  if (buttonA.getSingleDebouncedPress()) {
    running = !running;
    if (running) {
      motors.setSpeeds(150, 150);
      ledGreen(true);  // Romi on-board green LED (true=on)
      Serial.println("RUNNING");
    } else {
      motors.setSpeeds(0, 0);
      ledGreen(false);  // Romi on-board green LED (true=on)
      Serial.println("STOPPED");
    }
  }
}
