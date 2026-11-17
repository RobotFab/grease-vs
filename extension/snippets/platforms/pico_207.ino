// snippet: pico_207
// tags: pico, rp2040, raspberry-pi
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: RPi Pico: GPIO interrupt on pin 7 (INPUT_PULLUP)
// prompts: interrupt Pico, GPIO interrupt RP2040, attachInterrupt Raspberry Pi Pico, button interrupt Pico, edge count Pi Pico, external interrupt RP2040, volatile counter Pico
// verified: compile-only
// source: kd-generated (id 207)
// exact-include: Arduino.h

#include <Arduino.h>

volatile int edgeCount = 0;
unsigned long prevMs = 0;

void onRisingEdge() {
  edgeCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(7, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(7), onRisingEdge, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 1000) {
    prevMs = now;
    noInterrupts();
    int count = edgeCount;
    edgeCount = 0;
    interrupts();
    Serial.print("Edges: ");
    Serial.println(count);
  }
}
