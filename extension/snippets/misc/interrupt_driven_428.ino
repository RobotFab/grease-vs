// snippet: interrupt_driven_428
// tags: interrupt, attachinterrupt, isr, encoder, quadrature, pin2, pin3
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Implement a quadrature encoder reader on pins 2 (A) and 3 (B) using attachInterrupt() on both pins
// prompts: implement, quadrature, encoder, reader, attachinterrupt(), increment
// verified: compile-only
// source: kd-generated (id 428)
// exact-include: Arduino.h

#include <Arduino.h>

volatile long encoderCount = 0;
unsigned long prevMs = 0;

void onEncoderA() {
  if (digitalRead(3) == LOW) encoderCount++;
  else encoderCount--;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), onEncoderA, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 200) {  // 5 Hz
    prevMs = now;
    noInterrupts();
    long count = encoderCount;
    interrupts();
    Serial.print("Encoder count: ");
    Serial.println(count);
  }
}
