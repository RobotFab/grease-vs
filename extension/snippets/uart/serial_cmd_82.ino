// snippet: serial_cmd_82
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Serial command with float argument: SET_SPEED <value 0
// prompts: command, float, argument, set_speed, <value, clamp
// verified: compile-only
// source: kd-generated (id 82)
// exact-include: Arduino.h

#include <Arduino.h>

float motorSpeed = 0.0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("SET_SPEED")) {
      // "SET_SPEED" is 9 chars — substring(9) starts at the space+value
      float value = cmd.substring(9).toFloat();
      motorSpeed = constrain(value, 0.0, 1.0);
      analogWrite(5, motorSpeed * 255);
      Serial.print("SET_SPEED: "); Serial.println(motorSpeed);
    }
  }
}
