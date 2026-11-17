// snippet: serial_cmd_77
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Parse serial commands: PING replies PONG
// prompts: parse, serial, commands, replies, replies, grease
// verified: compile-only
// source: kd-generated (id 77)
// exact-include: Arduino.h

#include <Arduino.h>

char cmdBuf[32] = "";
int cmdIdx = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: PING, WHO, VER");
}

void processCommand(const char* cmd) {
  if (strcmp(cmd, "PING") == 0) {
    Serial.println("PONG");
  } else if (strcmp(cmd, "WHO") == 0) {
    Serial.println("GREASE");
  } else if (strcmp(cmd, "VER") == 0) {
    Serial.println("1.0");
  } else {
    Serial.print("ERR unknown: ");
    Serial.println(cmd);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      cmdBuf[cmdIdx] = '\0';
      processCommand(cmdBuf);
      cmdIdx = 0;
    } else if (c != '\r' && cmdIdx < 31) {
      cmdBuf[cmdIdx++] = c;
    }
  }
}
