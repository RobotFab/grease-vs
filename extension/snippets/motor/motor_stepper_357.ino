// snippet: motor_stepper_357
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Differential drive with two AccelStepper motors: left (step=3,dir=4), right (step=5,dir=6)
// prompts: differential, drive, accelstepper, motors, (step=3, dir=4)
// verified: compile-only
// source: kd-generated (id 357)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper leftStepper(AccelStepper::DRIVER, 3, 4);
AccelStepper rightStepper(AccelStepper::DRIVER, 5, 6);

void setup() {
  Serial.begin(9600);
  leftStepper.setMaxSpeed(1000);
  leftStepper.setAcceleration(500);
  rightStepper.setMaxSpeed(1000);
  rightStepper.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    handleCommand(command);
  }
  leftStepper.run();  // MUST be called every loop iteration to step the motor
  rightStepper.run();  // MUST be called every loop iteration to step the motor
}

void handleCommand(String command) {
  if (command.startsWith("F")) {
    int steps = command.substring(1).toInt();
    leftStepper.moveTo(steps);  // absolute target position (steps); call ONCE, not every loop
    rightStepper.moveTo(steps);  // absolute target position (steps); call ONCE, not every loop
  } else if (command.startsWith("R")) {
    int steps = command.substring(1).toInt();
    leftStepper.moveTo(-steps);  // absolute target position (steps); call ONCE, not every loop
    rightStepper.moveTo(-steps);  // absolute target position (steps); call ONCE, not every loop
  } else if (command.startsWith("L")) {
    int steps = command.substring(1).toInt();
    leftStepper.moveTo(-steps);  // absolute target position (steps); call ONCE, not every loop
    rightStepper.moveTo(steps);  // absolute target position (steps); call ONCE, not every loop
  } else if (command == "S") {
    leftStepper.moveTo(0);  // absolute target position (steps); call ONCE, not every loop
    rightStepper.moveTo(0);  // absolute target position (steps); call ONCE, not every loop
  }
}
