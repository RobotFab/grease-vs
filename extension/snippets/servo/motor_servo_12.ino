// snippet: motor_servo_12
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Control servo on pin 9 via serial command SET <angle 0-180>
// prompts: control, servo, serial, command, <angle, 0-180>
// verified: compile-only
// source: kd-generated (id 12)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int currentAngle = 90;

void setup() {
  Serial.begin(9600);
  myServo.attach(9);
  myServo.write(currentAngle);
  Serial.println("Servo control ready");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("SET ")) {
      int angle = command.substring(4).toInt();
      if (angle >= 0 && angle <= 180) {
        myServo.write(angle);
        currentAngle = angle;
        Serial.print("Servo moved to: ");
        Serial.println(currentAngle);
      } else {
        Serial.println("ERR: Angle out of range");
      }
    }
  }
}
