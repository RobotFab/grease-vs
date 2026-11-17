// snippet: serial_cmd_81
// tags: serial, command, parser, uart
// boards: avr, megaavr, romi32u4
// library: none
// description: CSV serial input: receive three comma-separated integers like 150,200,50 (three speeds)
// prompts: serial, input, receive, three, comma-separated, integers
// verified: compile-only
// source: kd-generated (id 81)
// exact-include: Arduino.h

#include <Arduino.h>

char cmdBuf[64];
int speeds[3];

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', cmdBuf, sizeof(cmdBuf));
    if (bytesRead > 0) {
      cmdBuf[bytesRead] = '\0';  // Null-terminate the string
      parseSpeeds(cmdBuf);
      applySpeeds();
      Serial.println("APPLIED");
    }
  }
}

void parseSpeeds(char* str) {
  // strtok() modifies its input in-place, so the argument must be a mutable
  // char* — passing a const char* is a compile error.
  int i = 0;
  char* token = strtok(str, ",");
  while (token != NULL && i < 3) {
    speeds[i] = atoi(token);
    token = strtok(NULL, ",");
    i++;
  }
}

void applySpeeds() {
  analogWrite(5, speeds[0]);
  analogWrite(6, speeds[1]);
  analogWrite(9, speeds[2]);
}
