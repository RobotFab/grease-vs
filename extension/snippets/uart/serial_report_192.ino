// snippet: serial_report_192
// tags: serial, report, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: On FLASH <n> command, blink LED on pin 13 exactly n times at 200ms intervals using millis
// prompts: flash, command, blink, exactly, times, 200ms
// verified: compile-only
// source: kd-generated (id 192)
// exact-include: Arduino.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino.h>

unsigned long prevMs = 0;


int flashCount = 0;
int flashTarget = 0;
bool flashing = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil(';');
    if (cmd.startsWith("FLASH ")) {
      flashTarget = cmd.substring(6).toInt();
      flashing = true;
      Serial.println("FLASHING");
      flashCount = 0;
    }
  }

  if (flashing) {
    unsigned long now = millis();
    if (now - prevMs >= 200) {
      prevMs = now;
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      flashCount++;
      if (flashCount >= flashTarget) {
        flashing = false;
        Serial.println("DONE");
      }
    }
  }
}

