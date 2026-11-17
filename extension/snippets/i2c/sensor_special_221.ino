// snippet: sensor_special_221
// tags: sensor, i2c, load-cell
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read GP2Y1010AU dust sensor: LED on pin 4, analog on A0
// prompts: gp2y1010au, sensor, analog, declare, float, dustdensity=0
// verified: compile-only
// source: kd-generated (id 221)
// exact-include: Arduino.h

#include <Arduino.h>

const int LED_PIN = 4;
const int SENSOR_PIN = A0;
const unsigned long PULSE_DURATION = 280;  // 0.28ms — LED on before sampling
const unsigned long READ_DELAY = 40;       // 0.04ms — wait after LED on before ADC read
const unsigned long DELTA_TIME = 9680;     // ~9.68ms rest between pulses (10ms total cycle)
const unsigned long PRINT_INTERVAL = 1000; // 1s between serial prints

float dustDensity = 0.0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  static unsigned long lastPrintMs = 0;

  // --- GP2Y1010AU sampling: pulse LED, wait, read ADC, rest ---
  // The sensor requires a 0.32ms LED pulse, sampled at 0.28ms into the pulse,
  // followed by ~9.68ms off before the next pulse (10ms total cycle).
  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(PULSE_DURATION);
  delayMicroseconds(READ_DELAY);
  int sensorValue = analogRead(SENSOR_PIN);
  digitalWrite(LED_PIN, LOW);
  delayMicroseconds(DELTA_TIME);  // rest before next sample

  // Convert ADC value to dust density (mg/m³).
  // map() truncates float args to long internally — use float math instead
  // so sub-integer precision is preserved.
  float voltage = sensorValue * (5.0 / 1023.0);  // AVR: 10-bit, 5V ref
  // GP2Y1010AU formula: dust = 0.17 * voltage - 0.1 (approximate, calibrate per unit)
  dustDensity = 0.17 * voltage - 0.1;
  if (dustDensity < 0) dustDensity = 0;

  // --- Print every 1 second (use millis, not micros — 1000 micros = 1ms) ---
  unsigned long now = millis();
  if (now - lastPrintMs >= PRINT_INTERVAL) {
    lastPrintMs = now;
    Serial.print("dust density: ");
    Serial.print(dustDensity);
    Serial.println(" mg/m3");
  }
}
