// snippet: sensor_special_222
// tags: sensor, i2c, load-cell
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: none
// description: Read ACS712 current sensor on A0
// prompts: acs712, current, sensor, declare, float, currenta=0
// verified: compile-only
// source: kd-generated (id 222)
// exact-include: Arduino.h

#include <Arduino.h>

const int ACS_PIN = A0;
const float VREF = 5.0f;
const float SENSITIVITY = 0.185f;  // V/A: 0.185 for 5A, 0.100 for 20A, 0.066 for 30A
float zeroPoint = 512.0;  // Initial zero-point value
float currentA = 0.0;

void calibrateZero() {
  long sum = 0;
  for (int i = 0; i < 100; i++) { sum += analogRead(ACS_PIN); delay(1); }
  zeroPoint = (sum / 100.0f);
  Serial.print("Zero calibrated: "); Serial.println(zeroPoint);
}

void setup() {
  Serial.begin(9600);
  calibrateZero();
}

void loop() {
  static unsigned long lastPrintTime = 0;
  unsigned long now = millis();

  if (now - lastPrintTime >= 200) {
    lastPrintTime = now;

    int sensorValue = analogRead(ACS_PIN);
    float v = (sensorValue - zeroPoint) * (VREF / 1023.0f);
    currentA = v / SENSITIVITY;

    Serial.print("Current: "); Serial.print(currentA, 3); Serial.println(" A");
  }
}
