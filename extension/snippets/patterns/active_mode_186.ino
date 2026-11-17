// snippet: active_mode_186
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: RECORD ON starts sampling A0 into float samples[50] at 20Hz
// prompts: active, sketch, record, starts, sampling, float
// verified: compile-only
// source: kd-generated (id 186)

float samples[50];
int sampleCount = 0;
bool recording = false;

void setup() {
  Serial.begin(115200);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input == "RECORD ON") {
      recording = true;
      sampleCount = 0;
    } else if (input == "RECORD OFF") {
      recording = false;
    } else if (input == "DUMP") {
      for (int i = 0; i < sampleCount; i++) {
        Serial.print("samples["); Serial.print(i); Serial.print("]: "); Serial.println(samples[i]);
      }
    }
  }

  if (recording && sampleCount < 50) {
    if (now - prevMs >= 50) {  // 20Hz = 50ms interval
      prevMs = now;
      samples[sampleCount++] = analogRead(A0) / 1023.0;  // Normalize to 0.0 to 1.0
    }
  }
}
