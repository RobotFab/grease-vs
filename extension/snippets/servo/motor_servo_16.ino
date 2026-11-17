// snippet: motor_servo_16
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Servo playback: Servo on pin 9 replays a hardcoded sequence of 8 angles {0,30,60,90,120,150,180,90}
// prompts: servo, playback, servo, replays, hardcoded, sequence
// verified: compile-only
// source: kd-generated (id 16)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
const int angles[] = {0, 30, 60, 90, 120, 150, 180, 90};
const int numAngles = sizeof(angles) / sizeof(angles[0]);
int currentAngleIndex = 0;
unsigned long prevMs = 0;
const unsigned long HOLD_TIME = 400;  // 400ms hold time for each angle

void setup() {
  myServo.attach(9);
  myServo.write(angles[currentAngleIndex]);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= HOLD_TIME) {
    prevMs = now;
    currentAngleIndex++;
    if (currentAngleIndex >= numAngles) {
      currentAngleIndex = 0;  // Loop back to the start
    }
    myServo.write(angles[currentAngleIndex]);
  }
}
