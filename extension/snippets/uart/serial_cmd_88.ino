// snippet: serial_cmd_88
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Serial buffer dump: RECORD starts filling float samples[100], DUMP sends all 100 with index
// prompts: buffer, record, starts, filling, float, samples[100]
// verified: compile-only
// source: kd-generated (id 88)
// exact-include: Arduino.h

#include <Arduino.h>

// Global variables
float samples[100];
int sampleIdx = 0;
bool recording = false;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "RECORD") {
      recording = true;
      sampleIdx = 0;
      Serial.println("Recording started.");
    } else if (command == "DUMP") {
      recording = false;
      Serial.println("Dumping samples:");
      for (int i = 0; i < sampleIdx; i++) {
        Serial.print("Sample "); Serial.print(i); Serial.print(": "); Serial.println(samples[i]);
      }
    }
  }

  if (recording && sampleIdx < 100) {
    samples[sampleIdx] = analogRead(A0) / 1023.0;
    sampleIdx++;
  }
}
