// snippet: interrupt_driven_427
// tags: interrupt, attachinterrupt, isr, pulse, width, micros, rising, falling
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Measure pulse width on pin 2: record micros() on RISING interrupt, record again on FALLING, compu…
// prompts: measure, pulse, width, record, micros(), rising
// verified: compile-only
// source: kd-generated (id 427)
// exact-include: Arduino.h

#include <Arduino.h>

volatile unsigned long pulseStart = 0;
volatile unsigned long pulseEnd = 0;
volatile bool pulseStarted = false;
volatile bool pulseReady = false;   // ISR hands a completed pulse to loop()

// ISRs stay tiny: capture micros() and set a flag. No Serial here — Serial is
// slow and itself interrupt-driven, so printing inside an ISR can hang.
// Note: Only one ISR can be bound per pin on Arduino cores.
// To capture both edges, use CHANGE interrupt and track edge state.
void onChange() {
  if (digitalRead(2) == HIGH) {
    // Rising edge detected
    pulseStart = micros();
    pulseStarted = true;
  } else {
    // Falling edge detected
    if (pulseStarted) {
      pulseEnd = micros();
      pulseStarted = false;
      pulseReady = true;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  // Use CHANGE interrupt to capture both rising and falling edges
  // Only one ISR can be bound per pin on Arduino cores
  attachInterrupt(digitalPinToInterrupt(2), onChange, CHANGE);
}

void loop() {
  // Do the printing here, not in the ISR. Snapshot the shared values under a
  // guard first (torn-read-safe on 8-bit AVR).
  if (pulseReady) {
    noInterrupts();
    unsigned long start = pulseStart;
    unsigned long end = pulseEnd;
    pulseReady = false;
    interrupts();

    unsigned long pulseWidth = end - start;
    Serial.print("Pulse width: ");
    Serial.println(pulseWidth);
  }
}
