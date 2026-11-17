// snippet: interrupt_driven_440
// tags: interrupt, attachinterrupt, isr, rc, receiver, pwm, servo
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Implement a 4-channel RC receiver decoder: read PWM signals on pins 2,3,4,5
// prompts: implement, 4-channel, receiver, decoder, signals, interrupts
// verified: compile-only
// source: kd-generated (id 440)
// exact-include: Arduino.h

#include <Arduino.h>

volatile unsigned long pulseWidths[4] = {0, 0, 0, 0};
volatile bool isMeasuring[4] = {false, false, false, false};

void handleChannel0() {
  static unsigned long lastEdge = 0;
  unsigned long now = micros();
  if (isMeasuring[0]) {
    pulseWidths[0] = now - lastEdge;
  } else {
    lastEdge = now;
  }
  isMeasuring[0] = !isMeasuring[0];
}

void handleChannel1() {
  static unsigned long lastEdge = 0;
  unsigned long now = micros();
  if (isMeasuring[1]) {
    pulseWidths[1] = now - lastEdge;
  } else {
    lastEdge = now;
  }
  isMeasuring[1] = !isMeasuring[1];
}

void handleChannel2() {
  static unsigned long lastEdge = 0;
  unsigned long now = micros();
  if (isMeasuring[2]) {
    pulseWidths[2] = now - lastEdge;
  } else {
    lastEdge = now;
  }
  isMeasuring[2] = !isMeasuring[2];
}

void handleChannel3() {
  static unsigned long lastEdge = 0;
  unsigned long now = micros();
  if (isMeasuring[3]) {
    pulseWidths[3] = now - lastEdge;
  } else {
    lastEdge = now;
  }
  isMeasuring[3] = !isMeasuring[3];
}

void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), handleChannel0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), handleChannel1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(4), handleChannel2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(5), handleChannel3, CHANGE);
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  if (millis() - prevMs >= 100) {
    prevMs = millis();
    unsigned long widths[4];
    noInterrupts();
    widths[0] = pulseWidths[0];
    widths[1] = pulseWidths[1];
    widths[2] = pulseWidths[2];
    widths[3] = pulseWidths[3];
    interrupts();
    Serial.print("Channel 0: ");
    Serial.print(widths[0]);
    Serial.print(" us, ");
    Serial.print("Channel 1: ");
    Serial.print(widths[1]);
    Serial.print(" us, ");
    Serial.print("Channel 2: ");
    Serial.print(widths[2]);
    Serial.print(" us, ");
    Serial.print("Channel 3: ");
    Serial.print(widths[3]);
    Serial.println(" us");
  }
}
