// snippet: esp32_active_180
// tags: esp32, active, serial, command
// boards: esp32
// library: none
// description: ESP32 active sketch: RECORD ON buffers float samples[100] from pin 34 at 100Hz
// prompts: esp32, active, sketch, record, buffers, float
// verified: compile-only
// source: kd-generated (id 180)
// exact-include: Arduino.h

#include <Arduino.h>

#define SAMPLE_RATE 100       // Hz
#define SAMPLE_SIZE 100
#define SAMPLE_PIN 34

float samples[SAMPLE_SIZE];
int sampleIndex = 0;
bool recording = false;
unsigned long prevSampleMs = 0;
const unsigned long SAMPLE_INTERVAL = 1000 / SAMPLE_RATE;  // 10ms

void setup() {
  Serial.begin(115200);
  pinMode(SAMPLE_PIN, INPUT);
  Serial.println("Send: RECORD ON, RECORD OFF, DUMP");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "RECORD ON") {
      recording = true;
      sampleIndex = 0;
      prevSampleMs = millis();
      Serial.println("Recording started");
    } else if (command == "RECORD OFF") {
      recording = false;
      Serial.println("Recording stopped");
    } else if (command == "DUMP") {
      Serial.println("Dumping samples:");
      for (int i = 0; i < sampleIndex; i++) {
        Serial.print("samples[");
        Serial.print(i);
        Serial.print("]: ");
        Serial.println(samples[i]);
      }
    }
  }

  // Sample at controlled 100Hz rate (not as fast as loop allows)
  if (recording && sampleIndex < SAMPLE_SIZE) {
    unsigned long now = millis();
    if (now - prevSampleMs >= SAMPLE_INTERVAL) {
      prevSampleMs = now;
      // ESP32: 12-bit ADC, 3.3V reference
      samples[sampleIndex++] = analogRead(SAMPLE_PIN) * (3.3f / 4095.0f);
    }
  }
}
