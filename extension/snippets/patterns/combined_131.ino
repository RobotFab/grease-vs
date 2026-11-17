// snippet: combined_131
// tags: combined, multi-sensor, millis, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Multi-axis joystick: A0 X-axis, A1 Y-axis
// prompts: multi-axis, joystick, x-axis, y-axis, declare, globally
// verified: compile-only
// source: kd-generated (id 131)
// exact-include: Servo.h

#include <Servo.h>

Servo servoX;
Servo servoY;
int joyX, joyY;

void setup() {
  servoX.attach(9);
  servoY.attach(10);
  joyX = 512;
  joyY = 512;
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 50;  // 20Hz

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    joyX = analogRead(A0);
    joyY = analogRead(A1);

    int servoXAngle = map(joyX, 0, 1023, 0, 180);
    int servoYAngle = map(joyY, 0, 1023, 0, 180);

    servoX.write(servoXAngle);
    servoY.write(servoYAngle);
  }
}
