// snippet: interrupt_driven_432
// tags: avr/interrupt
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: avr/interrupt
// description: Detect a falling edge on pin 2 (e
// prompts: detect, falling, zero-crossing, detector), measure, between
// verified: compile-only
// source: kd-generated (id 432)
// exact-include: avr/interrupt.h

#include <avr/interrupt.h>

const int ZERO_CROSS_PIN = 2;
volatile unsigned long lastTime = 0;
volatile unsigned long period = 0;
volatile bool detected = false;

void setup() {
  Serial.begin(9600);
  pinMode(ZERO_CROSS_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), zeroCross, FALLING);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 1000) {
    prevMs = now;
    noInterrupts();
    bool wasDetected = detected;
    unsigned long p = period;
    detected = false;
    interrupts();
    if (wasDetected) {
      float frequency = 1000000.0 / p;  // Convert period to frequency
      Serial.print("Frequency: ");
      Serial.print(frequency);
      Serial.println(" Hz");
    }
  }
}

void zeroCross() {
  unsigned long now = micros();
  if (lastTime != 0) {
    period = now - lastTime;
    detected = true;
  }
  lastTime = now;
}
