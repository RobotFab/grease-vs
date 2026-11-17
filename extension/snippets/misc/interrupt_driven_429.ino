// snippet: interrupt_driven_429
// tags: interrupt, attachinterrupt, isr, debounce, button, micros
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// not-tags: romi32u4, romi
// description: Debounce a button on pin 2 in an ISR: record micros() on each interrupt, ignore edges within 50 m…
// prompts: debounce, button, record, micros(), interrupt, ignore
// verified: compile-only
// source: kd-generated (id 429)
// exact-include: Arduino.h

#include <Arduino.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

const int BUTTON_PIN = 2;
const int LED_PIN = LED_BUILTIN;
const unsigned long DEBOUNCE_MS = 50;

volatile unsigned long lastPressMicros = 0;
volatile bool pressPending = false;   // set in the ISR, cleared in loop()
bool ledState = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
}

void loop() {
  // Do the work HERE, not in the ISR. An ISR should set a flag and return; anything
  // longer blocks every other interrupt on the board — including the timer that
  // millis() and micros() run on.
  if (!pressPending) return;

  noInterrupts();
  pressPending = false;
  interrupts();

  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
  Serial.println("pressed");
}

// The ISR does exactly two things: reject bounce, and raise a flag.
void buttonISR() {
  unsigned long currentMicros = micros();
  if (currentMicros - lastPressMicros > DEBOUNCE_MS * 1000UL) {
    lastPressMicros = currentMicros;
    pressPending = true;
  }
}
