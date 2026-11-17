// snippet: motor_servo_20
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Servo position ramp: serial TARGET <angle> command, ramp current angle toward target at max 90 de…
// prompts: servo, position, serial, target, <angle>, command
// verified: compile-only
// source: kd-generated (id 20)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int currentAngle = 90;
int targetAngle = 90;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 10;  // 10ms step for 90deg/sec max

void setup() {
  Serial.begin(9600);
  myServo.attach(9);
  myServo.write(currentAngle);
  Serial.println("Servo ready. Use TARGET <angle> to set target angle.");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      if (command.startsWith("TARGET")) {
        targetAngle = command.substring(7).toInt();
        Serial.print("Target angle set to: "); Serial.println(targetAngle);
      }
    }

    int angleDiff = targetAngle - currentAngle;
    if (angleDiff != 0) {
      int step = (angleDiff > 0) ? 1 : -1;
      currentAngle += step;
      if ((angleDiff > 0 && currentAngle >= targetAngle) || (angleDiff < 0 && currentAngle <= targetAngle)) {
        currentAngle = targetAngle;
      }
      myServo.write(currentAngle);
    }
  }
}
