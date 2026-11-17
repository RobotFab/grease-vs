// snippet: motor_servo_18
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Servo idles at 90 degrees
// prompts: servo, idles, degrees, moves, digital, returns
// verified: compile-only
// source: kd-generated (id 18)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int targetPos = 90;
const unsigned long INTERVAL = 20;  // 50Hz = 1000ms / 50 = 20ms
unsigned long prevMs = 0;

void setup() {
  myServo.attach(9);
  pinMode(3, INPUT_PULLUP);
  myServo.write(targetPos);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    if (digitalRead(3) == LOW) {
      targetPos = 0;
    } else {
      targetPos = 90;
    }

    myServo.write(targetPos);
  }
}
