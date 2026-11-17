// snippet: sensor_hall_582
// tags: hall, magnetic, sensor, flow, meter, pulse, interrupt, liquid
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Hall-based liquid flow meter: count pulses from flow sensor on pin 2 (7.5 pulses/mL)
// prompts: hall-based, liquid, meter, count, pulses, sensor
// verified: compile-only
// source: kd-generated (id 582)
// exact-include: Arduino.h

#include <Arduino.h>

const int FLOW_PIN = 2;
const float PULSES_PER_ML = 7.5;
const unsigned long INTERVAL_MS = 1000;  // measurement interval

volatile unsigned long pulseCount = 0;
unsigned long lastPrintTime = 0;
float flowRate = 0.0;
float totalVolume = 0.0;

void pulseHandler() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulseHandler, FALLING);
}

void loop() {
  unsigned long now = millis();

  if (now - lastPrintTime >= INTERVAL_MS) {
    lastPrintTime = now;

    // Atomically grab and reset pulse count
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    // Flow rate = pulses / (interval in seconds) / pulses-per-mL
    if (count > 0) {
      flowRate = (float)count / (INTERVAL_MS / 1000.0) / PULSES_PER_ML;
      totalVolume += (float)count / PULSES_PER_ML;
    } else {
      flowRate = 0.0;  // no flow detected
    }

    Serial.print("Flow Rate: "); Serial.print(flowRate, 2); Serial.println(" mL/s");
    Serial.print("Total Volume: "); Serial.print(totalVolume, 2); Serial.println(" mL");
  }
}
