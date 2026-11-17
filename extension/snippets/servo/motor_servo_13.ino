// snippet: motor_servo_13
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Pan-tilt: Servo pan on pin 9, Servo tilt on pin 10
// prompts: pan-tilt, servo, servo, declare, scope, commands
// verified: compile-only
// source: kd-generated (id 13)
// exact-include: Servo.h

#include <Servo.h>

Servo panServo;   // left-right
Servo tiltServo;  // up-down

int panAngle  = 90;
int tiltAngle = 90;

void setup() {
  Serial.begin(9600);
  panServo.attach(9);
  tiltServo.attach(10);
  panServo.write(panAngle);
  tiltServo.write(tiltAngle);
  Serial.println("Pan-Tilt control ready");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("PAN")) {
      int angle = command.substring(4).toInt();
      panAngle = constrain(angle, 0, 180);
      panServo.write(panAngle);
      Serial.print("PAN: "); Serial.println(panAngle);
    } else if (command.startsWith("TILT")) {
      int angle = command.substring(5).toInt();
      tiltAngle = constrain(angle, 0, 180);
      tiltServo.write(tiltAngle);
      Serial.print("TILT: "); Serial.println(tiltAngle);
    } else if (command == "CENTER") {
      panAngle = 90;
      tiltAngle = 90;
      panServo.write(panAngle);
      tiltServo.write(tiltAngle);
      Serial.println("CENTERED");
    }
  }
}
