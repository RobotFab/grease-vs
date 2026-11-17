// snippet: misc_156
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Non-blocking LED blink pattern: declare uint16_t pattern=0b1010100110011010, int bitIdx=0, unsign…
// prompts: non-blocking, blink, pattern, uint16_t, pattern=0b1010100110011010, bitidx=0
// verified: compile-only
// source: kd-generated (id 156)
// exact-include: Arduino.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino.h>

uint16_t pattern = 0b1010100110011010;
int bitIdx = 0;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 100;  // 100ms per bit

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    bitIdx = (bitIdx + 1) % 16;  // Shift to next bit
    digitalWrite(LED_BUILTIN, bitRead(pattern, bitIdx));
  }
}
