// snippet: serial_report_194
// tags: serial, report, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Serial plotter for motor tuning: print setpoint and actual speed (encoder count/sec) as labeled C…
// prompts: plotter, motor, tuning, setpoint, actual, speed
// verified: compile-only
// source: kd-generated (id 194)
// exact-include: math.h

#include <math.h>

float setpoint = 0.0f;
float actualSpeed = 0.0f;
volatile int encCount = 0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);  // Encoder A
  pinMode(3, INPUT_PULLUP);  // Encoder B
  attachInterrupt(digitalPinToInterrupt(2), onEncoderA, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;  // 20 Hz
  prevMs = now;

  // Calculate actual speed (encoder count/sec)
  noInterrupts();
  int count = encCount;
  encCount = 0;  // Reset encoder count for next cycle
  interrupts();
  actualSpeed = count * 20.0f;  // 20 Hz update rate

  // Format: "setpoint:actualSpeed\n"
  Serial.print("setpoint:"); Serial.print(setpoint, 2);
  Serial.print(",actualSpeed:"); Serial.println(actualSpeed, 2);
}

void onEncoderA() {
  if (digitalRead(3) == HIGH) encCount++;
  else encCount--;
}
