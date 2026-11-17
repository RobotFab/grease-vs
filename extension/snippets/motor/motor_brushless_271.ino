// snippet: motor_brushless_271
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: ESC brushless motor on pin 9
// prompts: brushless, motor, include, servo, declare, servo
// verified: compile-only
// source: kd-generated (id 271)
// exact-include: Servo.h

#include <Servo.h>

const int ESC_PIN = 9;
Servo esc;

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
    if (command.startsWith("THROTTLE")) {
      int throttle = command.substring(8).toInt();
      if (throttle >= 0 && throttle <= 100) {
        esc.writeMicroseconds(1000 + throttle * 10);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
      }
    } else if (command == "STOP") {
      esc.writeMicroseconds(1000);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
    }
  }
}
