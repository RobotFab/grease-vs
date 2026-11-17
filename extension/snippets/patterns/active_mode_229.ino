// snippet: active_mode_229
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: FREQ <hz> plays tone on pin 8 for 500ms using millis, non-blocking, silences after
// prompts: active, sketch, plays, 500ms, non-blocking, silences
// verified: compile-only
// source: kd-generated (id 229)
// exact-include: Arduino.h

#include <Arduino.h>

#define BUZZER_PIN 8
#define TONE_DURATION 500

unsigned long toneEnd = 0;
bool toneActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Send: FREQ <hz>  or  STOP  or  PING");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("FREQ")) {
      // Parse: FREQ <hz>
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        int freq = command.substring(spaceIdx + 1).toInt();
        if (freq > 0) {
          toneActive = true;
          toneEnd = millis() + TONE_DURATION;
          tone(BUZZER_PIN, freq);
        }
      }
    } else if (command == "STOP") {
      toneActive = false;
      noTone(BUZZER_PIN);
    } else if (command == "PING") {
      Serial.println("PONG");
    }
  }

  // Auto-silence after duration (non-blocking)
  if (toneActive && millis() >= toneEnd) {
    toneActive = false;
    noTone(BUZZER_PIN);
  }
}
