// snippet: sensor_hall_576
// tags: hall, magnetic, sensor, interrupt, pulse, rpm, counter, digital
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Count pulses from a digital Hall effect sensor on pin 2 (interrupt, RISING)
// prompts: count, pulses, digital, effect, sensor, (interrupt
// verified: compile-only
// source: kd-generated (id 576)
// exact-include: Arduino.h

#include <Arduino.h>

const int HALL_PIN = 2;
volatile unsigned long pulseCount = 0;
unsigned long lastRPMTime = 0;
unsigned long RPM = 0;

void onPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), onPulse, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - lastRPMTime >= 500) {
    lastRPMTime = now;
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();
    RPM = count * 60;  // 1 magnet per revolution
    Serial.print("RPM: ");
    Serial.println(RPM);
  }
}
