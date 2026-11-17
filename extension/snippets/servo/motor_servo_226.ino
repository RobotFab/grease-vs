// snippet: motor_servo_226
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Four-servo arm: servos on pins 3/5/6/9 (base, shoulder, elbow, wrist)
// prompts: four-servo, servos, 3/5/6/9, (base, shoulder, elbow
// verified: compile-only
// source: kd-generated (id 226)
// exact-include: Servo.h

#include <Servo.h>

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist;

void setup() {
  Serial.begin(9600);
  base.attach(3);
  shoulder.attach(5);
  elbow.attach(6);
  wrist.attach(9);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("JOINT")) {
      int joint = command.charAt(6) - '0';
      int angle = command.substring(8).toInt();
      if (joint >= 1 && joint <= 4) {
        switch (joint) {
          case 1: base.write(angle); break;
          case 2: shoulder.write(angle); break;
          case 3: elbow.write(angle); break;
          case 4: wrist.write(angle); break;
        }
      }
    }
  }
}
