// snippet: interrupt_driven_439
// tags: interrupt, attachinterrupt, isr, pwm, duty-cycle, micros, rising, falling
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Measure the duty cycle of a PWM signal on pin 2 by recording RISING and FALLING timestamps with m…
// prompts: measure, cycle, signal, recording, rising, falling
// verified: compile-only
// source: kd-generated (id 439)
// exact-include: Arduino.h

#include <Arduino.h>

volatile unsigned long risingTime = 0;
volatile unsigned long fallingTime = 0;
volatile bool isRising = false;

// Note: Only one ISR can be bound per pin on Arduino cores.
// To capture both edges, use CHANGE interrupt and track edge state.
void handleChange() {
  if (digitalRead(2) == HIGH) {
    // Rising edge detected
    risingTime = micros();
    isRising = true;
  } else {
    // Falling edge detected
    fallingTime = micros();
    isRising = false;
  }
}

void setup() {
  pinMode(2, INPUT);
  // Use CHANGE interrupt to capture both rising and falling edges
  // Only one ISR can be bound per pin on Arduino cores
  attachInterrupt(digitalPinToInterrupt(2), handleChange, CHANGE);
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  if (millis() - prevMs >= 1000) {
    prevMs = millis();
    noInterrupts();
    unsigned long rt = risingTime;
    unsigned long ft = fallingTime;
    risingTime = 0;
    fallingTime = 0;
    interrupts();
    if (rt != 0 && ft != 0) {
      unsigned long period = ft - rt;
      unsigned long dutyCycle = (ft - rt) * 100 / period;
      Serial.print("Duty Cycle: ");
      Serial.print(dutyCycle);
      Serial.println("%");
    }
  }
}
