// snippet: esp32_touch
// tags: esp32, touch, touchread, capacitive, t0, touchpin, threshold
// boards: esp32
// library: none
// description: Read the ESP32 built-in capacitive touch sensor with touchRead(T0), millis-based.
//   ESP32 touch pins are BUILT-IN — no external chip or library needed.
//   touchRead(T0) returns a value that DROPS when the pin is touched (lower = touched).
//   T0 = GPIO4, T3 = GPIO15, T4 = GPIO13, T5 = GPIO12, T6 = GPIO14, T7 = GPIO27.
//   NOT: Adafruit_MPR121 — that is an external I2C breakout, not the ESP32 built-in touch.
//   NOT: analogRead(T0) — use touchRead().
// prompts: ESP32 touch sensor, touchRead T0, ESP32 capacitive touch pin, ESP32 touch threshold LED, read touch pin ESP32, ESP32 built-in touch
// not-tags: mpr121
// exact-include: Arduino.h

#include <Arduino.h>

const int LED_PIN = 2;
const int TOUCH_THRESHOLD = 40;  // touchRead value drops below this when touched
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  int value = touchRead(T0);  // T0 = GPIO4
  Serial.print("touch: ");
  Serial.println(value);
  digitalWrite(LED_PIN, value < TOUCH_THRESHOLD ? HIGH : LOW);
}
