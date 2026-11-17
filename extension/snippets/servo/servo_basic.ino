// snippet: servo_basic
// tags: servo, motor, angle, pwm, rc, position, sweep
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Attach a servo on pin 9 and sweep 0-180 degrees using Servo.h
// prompts: servo motor, rotate servo, servo angle, sweep servo, servo position, RC servo, control servo degrees, Servo.write
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
unsigned long prevMs = 0;
int angle = 0;
int direction = 1;

void setup() {
  myServo.attach(9);
  myServo.write(90);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  myServo.write(angle);
  angle += direction;
  if (angle >= 180 || angle <= 0) direction = -direction;
}
