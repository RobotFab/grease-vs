// snippet: serial_cmd_191
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Multi-command protocol: commands terminate with semicolon
// prompts: multi-command, protocol, commands, terminate, semicolon, accumulate
// verified: compile-only
// source: kd-generated (id 191)
// exact-include: Arduino.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino.h>

char cmdBuf[64];
int cmdIdx = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: LED_ON; LED_OFF; SPEED:<value>;");
}

void processCommand(const String& cmd) {
  if (cmd == "LED_ON")  { digitalWrite(LED_BUILTIN, HIGH); Serial.println("DONE"); }
  else if (cmd == "LED_OFF") { digitalWrite(LED_BUILTIN, LOW); Serial.println("DONE"); }
  else if (cmd.startsWith("SPEED:")) {
    int val = cmd.substring(6).toInt();
    Serial.print("DONE SPEED="); Serial.println(val);
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == ';') {
      cmdBuf[cmdIdx] = '\0';
      processCommand(String(cmdBuf));
      cmdIdx = 0;
    } else if (cmdIdx < 63) {
      cmdBuf[cmdIdx++] = c;
    }
  }
}
