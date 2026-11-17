// snippet: interrupt_driven_436
// tags: interrupt, attachinterrupt, isr, tachometer, hall, rpm, counter
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Implement a tachometer: count pulses from a Hall effect sensor on pin 2 via interrupt
// prompts: implement, tachometer, count, pulses, effect, sensor
// verified: compile-only
// source: kd-generated (id 436)
// exact-include: Arduino.h

#include <Arduino.h>

volatile unsigned long pulseCount = 0;
unsigned long prevMs = 0;

void onPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), onPulse, FALLING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();
    float rpm = (count * 60.0) / 1;  // 1 magnet per revolution
    Serial.print("RPM: ");
    Serial.println(rpm);
  }
}
