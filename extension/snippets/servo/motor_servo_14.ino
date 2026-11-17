// snippet: motor_servo_14
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Servo on pin 9 tracks potentiometer on A0 mapped 0–180 degrees
// prompts: servo, tracks, potentiometer, mapped, 0–180, degrees
// verified: compile-only
// source: kd-generated (id 14)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int targetAngle = 90;

void setup() {
  myServo.attach(9);
  myServo.write(targetAngle);
}

void loop() {
  static unsigned long lastUpdate = 0;
  const unsigned long UPDATE_INTERVAL = 50;  // 20Hz

  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;
    int potValue = analogRead(A0);
    targetAngle = map(potValue, 0, 1023, 0, 180);
    myServo.write(targetAngle);
  }
}
