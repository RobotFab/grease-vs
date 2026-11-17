// snippet: serial_cmd_78
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Serial command dispatcher: read line, split on first space, first token is command name
// prompts: command, dispatcher, split, first, space, first
// verified: compile-only
// source: kd-generated (id 78)
// exact-include: Arduino.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino.h>

char cmdBuf[64] = "";
int cmdIdx = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: LED <state>, PWM <duty>, RESET");
}

void processCommand(const char* cmd) {
  char* token = strtok((char*)cmd, " ");
  if (token == NULL) return;

  if (strcmp(token, "LED") == 0) {
    token = strtok(NULL, " ");
    if (token != NULL) {
      if (strcmp(token, "ON") == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("LED ON");
      } else if (strcmp(token, "OFF") == 0) {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("LED OFF");
      }
    }
  } else if (strcmp(token, "PWM") == 0) {
    token = strtok(NULL, " ");
    if (token != NULL) {
      int duty = atoi(token);
      analogWrite(LED_BUILTIN, duty);
      Serial.print("PWM set to: ");
      Serial.println(duty);
    }
  } else if (strcmp(token, "RESET") == 0) {
    Serial.println("Resetting...");
    delay(1000); // Simulate reset
    Serial.println("Reset complete");
  } else {
    Serial.print("ERR unknown command: ");
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
    } else if (c != '\r' && cmdIdx < 63) {
      cmdBuf[cmdIdx++] = c;
    }
  }
}
