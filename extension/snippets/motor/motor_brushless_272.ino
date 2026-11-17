// snippet: motor_brushless_272
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: ESC throttle ramp: Include Servo
// prompts: throttle, include, servo, declare, servo, globally
// verified: compile-only
// source: kd-generated (id 272)
// exact-include: Servo.h

#include <Servo.h>

const int ESC_PIN = 9;
Servo esc;
int currentUs = 1000;

void armESC() {
  esc.writeMicroseconds(1000);  // minimum throttle
  unsigned long start = millis();
  while (millis() - start < 2000) {
    // Wait for 2 seconds for the ESC to arm
  }
}

void setup() {
  Serial.begin(9600);
  esc.attach(ESC_PIN, 1000, 2000);  // min 1000µs, max 2000µs
  armESC();
  Serial.println("ESC armed");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("RAMP")) {
      int targetPct = command.substring(5).toInt();
      if (targetPct >= 0 && targetPct <= 100) {
        int targetUs = 1000 + targetPct * 10;
        unsigned long start = millis();
        while (millis() - start < abs(targetUs - currentUs) * 20 / 10) {
          if (targetUs > currentUs) {
            currentUs += 10;
          } else {
            currentUs -= 10;
          }
          esc.writeMicroseconds(currentUs);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
          Serial.print("Current Throttle: "); Serial.println(currentUs - 1000);
          delay(20);
        }
      }
    }
  }
}
