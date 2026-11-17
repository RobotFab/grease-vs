// snippet: motor_servo_19
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Dual servo gripper on pins 9 and 10
// prompts: servo, gripper, command, <percent, 0-100>, gripper
// verified: compile-only
// source: kd-generated (id 19)
// exact-include: Servo.h

#include <Servo.h>

Servo gripper1;
Servo gripper2;
int angle1 = 90;
int angle2 = 90;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 100;

void setup() {
  Serial.begin(9600);
  gripper1.attach(9);
  gripper2.attach(10);
  gripper1.write(angle1);
  gripper2.write(angle2);
  Serial.println("Gripper ready. Use GRIP <percent> to set angle.");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      if (command.startsWith("GRIP")) {
        int percent = command.substring(5).toInt();
        angle1 = map(percent, 0, 100, 0, 180);
        angle2 = 180 - angle1;
        gripper1.write(angle1);
        gripper2.write(angle2);
        Serial.print("Gripper angle set to: "); Serial.println(angle1);
      }
    }
  }
}
