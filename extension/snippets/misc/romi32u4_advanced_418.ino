// snippet: romi32u4_advanced_418
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Receive drive commands over USB Serial: 'F' forward, 'B' backward, 'L' left, 'R' right, 'S' stop
// prompts: receive, drive, commands, forward, backward, right
// verified: compile-only
// source: kd-generated (id 418)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
// Onboard green LED is a FREE FUNCTION: ledGreen(bool) — not digitalWrite on pin 13.

char receivedCommand = '\0';
unsigned long commandStartTime = 0;
const unsigned long COMMAND_DURATION = 500;  // 500 ms

void setup() {
  Serial.begin(9600);
  motors.setSpeeds(0, 0);
}

void loop() {
  if (Serial.available() > 0) {
    receivedCommand = Serial.read();
    commandStartTime = millis();
    ledGreen(true);
  }

  if (receivedCommand != '\0') {
    unsigned long now = millis();
    if (now - commandStartTime >= COMMAND_DURATION) {
      motors.setSpeeds(0, 0);
      receivedCommand = '\0';
      ledGreen(false);
    } else {
      switch (receivedCommand) {
        case 'F':
          motors.setSpeeds(150, 150);
          break;
        case 'B':
          motors.setSpeeds(-150, -150);
          break;
        case 'L':
          motors.setSpeeds(-150, 150);
          break;
        case 'R':
          motors.setSpeeds(150, -150);
          break;
        case 'S':
          motors.setSpeeds(0, 0);
          break;
      }
    }
  }
}
