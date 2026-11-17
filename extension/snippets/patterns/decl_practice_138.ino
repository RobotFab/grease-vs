// snippet: decl_practice_138
// tags: globals, declare, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Declare char cmdBuf[32] and int cmdIdx=0 at global scope
// prompts: declare, cmdbuf[32], cmdidx=0, scope, command, parser
// verified: compile-only
// source: kd-generated (id 138)
// exact-include: Arduino.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino.h>

char cmdBuf[32] = "";
int cmdIdx = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: LED_ON, LED_OFF, SPEED:<value>");
  pinMode(LED_BUILTIN, OUTPUT);
}

void processCommand(const char* cmd) {
  if (strcmp(cmd, "LED_ON") == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("OK LED_ON");
  } else if (strcmp(cmd, "LED_OFF") == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("OK LED_OFF");
  } else if (strncmp(cmd, "SPEED:", 6) == 0) {
    int val = atoi(cmd + 6);
    Serial.print("OK SPEED=");
    Serial.println(val);
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
