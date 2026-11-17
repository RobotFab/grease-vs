// snippet: motor_servo_11
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Sweep servo on pin 9 from 0 to 180 and back over 2 seconds using millis
// prompts: sweep, servo, seconds, declare, servo, myservo
// verified: compile-only
// source: kd-generated (id 11)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int servoPos = 0;
int servoDir = 1;
unsigned long prevMs = 0;
const unsigned long STEP_MS = 10;  // one degree every 10ms = 1.8s full sweep

void setup() {
  myServo.attach(9);
  myServo.write(0);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= STEP_MS) {
    prevMs = now;
    servoPos += servoDir;
    if (servoPos >= 180 || servoPos <= 0) servoDir = -servoDir;
    myServo.write(servoPos);
  }
}
