// snippet: esp32_ledc_pwm
// tags: esp32, ledc, pwm, motor, dimming, analogwrite, ledcAttach, ledcWrite
// boards: esp32
// library: none
// description: ESP32 PWM via LEDC API (core 3.x) — ledcAttach/ledcWrite; analogWrite() does not exist on ESP32.
//   WRONG on ESP32: analogWrite(pin, duty)   — undefined on ESP32 core
//   RIGHT (core 3.x): ledcAttach(pin, freq, res)  — attach pin with freq and resolution
//                      ledcWrite(pin, duty)        — set duty (0–255 for 8-bit)
//   DEPRECATED (core 2.x): ledcSetup/ledcAttachPin — removed in ESP32 core 3.x
//   ledcAttach() returns false if the pin cannot be attached (no free LEDC channel, or an
//   input-only GPIO 34-39) — check it, or every later ledcWrite() is silently discarded.
//   Ramp the duty from loop() on a millis() step, not with delay(). A motor sketch that sits
//   inside `for (duty...) delay(20)` cannot read a limit switch, an e-stop or a serial command
//   for the whole 2 s of the ramp.
//   5 kHz is a good default for a brushed motor: above the ~2 kHz audible whine, below the
//   switching losses of a cheap H-bridge. Drop to 1 kHz for high-inductance motors.
// prompts: ESP32 PWM, ESP32 LEDC, ledcWrite ESP32, ESP32 LED dimming, ESP32 motor PWM, analogWrite ESP32, ESP32 PWM LED, LEDC channel ESP32, ESP32 fade LED, ESP32 PWM H-bridge, ESP32 PWM speed control, ledcSetup ESP32, PWM ESP32 8-bit
// not-tags: arduino:avr, rp2040, atmega, samd
// exact-include: Arduino.h

#include <Arduino.h>

const int PWM_PIN = 25;
const int FREQ    = 5000;  // Hz
const int RES     = 8;     // bits (0–255)

// Motor direction pin (for H-bridge)
const int DIR_PIN = 26;

int duty    = 0;
int dutyDir = 5;               // +5 while ramping up, -5 while ramping down
const unsigned long STEP_MS = 20;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, HIGH);  // forward

  // ESP32 core 3.x API: ledcAttach(pin, freq, resolution)
  if (!ledcAttach(PWM_PIN, FREQ, RES)) {
    Serial.println("ledcAttach failed — bad pin or no free LEDC channel");
    while (1);   // halt: `return` here would leave loop() writing duty to nothing
  }
  ledcWrite(PWM_PIN, 0);        // start stopped, never at an inherited duty
}

void loop() {
  if (millis() - prevMs < STEP_MS) return;
  prevMs = millis();

  duty += dutyDir;
  if (duty >= 255) { duty = 255; dutyDir = -5; }
  if (duty <= 0)   { duty = 0;   dutyDir =  5; }

  ledcWrite(PWM_PIN, duty);     // 0-255 for RES = 8 bits
}
