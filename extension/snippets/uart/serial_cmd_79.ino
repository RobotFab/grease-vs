// snippet: serial_cmd_79
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Receive SET_PIN <pin> <state> over serial
// prompts: receive, set_pin, <pin>, <state>, serial, validate
// verified: compile-only
// source: kd-generated (id 79)
// exact-include: Arduino.h

#include <Arduino.h>

char cmdBuf[32];
int cmdIdx = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      cmdBuf[cmdIdx] = '\0';
      handleCommand(cmdBuf);
      cmdIdx = 0;
    } else if (cmdIdx < 31) {
      cmdBuf[cmdIdx++] = c;
    }
  }
}

void handleCommand(const char* cmd) {
  int pin, state;
  if (sscanf(cmd, "SET_PIN %d %d", &pin, &state) == 2) {
    if (pin >= 2 && pin <= 13) {
      digitalWrite(pin, state);
      Serial.println("OK");
    } else {
      Serial.println("ERR invalid pin");
    }
  } else {
    Serial.println("ERR invalid command");
  }
}
