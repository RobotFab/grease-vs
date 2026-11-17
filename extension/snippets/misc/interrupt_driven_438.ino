// snippet: interrupt_driven_438
// tags: avr/sleep
// boards: avr, megaavr, romi32u4
// library: avr/sleep
// description: Use attachInterrupt() on pin 2 to wake the Uno from sleep (SLEEP_MODE_PWR_DOWN)
// prompts: attachinterrupt(), sleep, (sleep_mode_pwr_down), toggle, avr/sleep, library
// verified: compile-only
// source: kd-generated (id 438)
// exact-include: avr/sleep.h

#include <avr/sleep.h>

const int WAKE_PIN = 2;
const int LED_PIN = 13;
const unsigned long SLEEP_MS = 2000;

// Flag to indicate wake-up time has passed
volatile bool wakeTimePassed = false;
unsigned long wakeTime = 0;

void setup() {
  pinMode(WAKE_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(WAKE_PIN), wakeUp, RISING);
  sleep_enable();
  sleep_mode();
}

void loop() {
  // Main loop runs after wake-up
  // Note: delay() cannot be used inside ISRs on AVR because global interrupts
  // are disabled during ISR execution, and delay() depends on timer0 overflow
  // which requires interrupts to be enabled.
  
  if (wakeTimePassed) {
    // Wait for the specified sleep duration before going back to sleep
    if (millis() - wakeTime >= SLEEP_MS) {
      sleep_enable();
      sleep_mode();
      wakeTimePassed = false;
    }
  }
}

void wakeUp() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  sleep_disable();
  wakeTime = millis();
  wakeTimePassed = true;
  // Note: No delay() here - it would hang because interrupts are disabled in ISR
  // Instead, we use millis() in loop() to track time
}
