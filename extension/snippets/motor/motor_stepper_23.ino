// snippet: motor_stepper_23
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Two AccelStepper motors: stepper1 on pins 2/3, stepper2 on pins 4/5
// prompts: accelstepper, motors, stepper1, stepper2, move1, <pos>
// verified: compile-only
// source: kd-generated (id 23)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper1(AccelStepper::DRIVER, 2, 3);
AccelStepper stepper2(AccelStepper::DRIVER, 4, 5);

void setup() {
  Serial.begin(9600);
  stepper1.setMaxSpeed(1000);
  stepper1.setAcceleration(500);
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("MOVE1")) {
      int pos = command.substring(6).toInt();
      stepper1.moveTo(pos);  // absolute target position (steps); call ONCE, not every loop
    } else if (command.startsWith("MOVE2")) {
      int pos = command.substring(6).toInt();
      stepper2.moveTo(pos);  // absolute target position (steps); call ONCE, not every loop
    }
  }
  stepper1.run();  // MUST be called every loop iteration to step the motor
  stepper2.run();  // MUST be called every loop iteration to step the motor
}
