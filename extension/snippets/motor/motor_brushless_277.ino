// snippet: motor_brushless_277
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: ESC speed hold: Include Servo
// prompts: speed, include, servo, declare, servo, globally
// verified: compile-only
// source: kd-generated (id 277)
// exact-include: Servo.h

#include <Servo.h>

Servo esc;
int targetUs = 1000;
int currentUs = 1000;
const int ESC_PIN = 9;
const int MAX_ADJUSTMENT = 5;
const unsigned long INTERVAL = 20;  // 50Hz

void setup() {
  esc.attach(ESC_PIN, 1000, 2000);  // min 1000µs, max 2000µs
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    // Read target speed from serial (0-100)
    if (Serial.available() > 0) {
      int newSetpoint = Serial.parseInt();
      if (newSetpoint >= 0 && newSetpoint <= 100) {
        targetUs = map(newSetpoint, 0, 100, 1000, 2000);
      }
    }

    // Adjust currentUs toward targetUs
    int diff = targetUs - currentUs;
    if (diff > 0) {
      currentUs = min(currentUs + MAX_ADJUSTMENT, targetUs);
    } else if (diff < 0) {
      currentUs = max(currentUs - MAX_ADJUSTMENT, targetUs);
    }

    // Write currentUs to ESC
    esc.writeMicroseconds(currentUs);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
  }
}
