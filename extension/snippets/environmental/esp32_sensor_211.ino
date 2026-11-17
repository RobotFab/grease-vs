// snippet: esp32_sensor_211
// tags: esp32, sensor, millis
// boards: esp32
// library: none
// description: ESP32: read MQ-2 gas sensor on pin 34
// prompts: esp32, sensor, declare, float, ppm=0, globally
// verified: compile-only
// source: kd-generated (id 211)
// exact-include: Arduino.h

#include <Arduino.h>

const int MQ2_PIN = 34;
const int LED_PIN = 2;
const unsigned long INTERVAL = 500;
float ppm = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= INTERVAL) {
    t = millis();
    int raw = analogRead(MQ2_PIN);
    ppm = calculatePPM(raw);  // Placeholder function to calculate PPM
    Serial.print("PPM: "); Serial.println(ppm);

    if (ppm > 500) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("ALERT");
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

float calculatePPM(int raw) {
  // Placeholder calculation for PPM
  // Replace with actual PPM calculation based on sensor datasheet
  return raw * 0.1;  // Example calculation
}
