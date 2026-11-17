// snippet: motor_stepper_351
// tags: motor, stepper, direction, accelstepper, multistepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper, MultiStepper
// description: Two-axis XY motion with AccelStepper: X (step=3,dir=4), Y (step=5,dir=6)
// prompts: two-axis, motion, accelstepper, (step=3, dir=4), (step=5
// verified: compile-only
// source: kd-generated (id 351)
// exact-include: AccelStepper.h, MultiStepper.h

#include <AccelStepper.h>
#include <MultiStepper.h>

AccelStepper stepperX(AccelStepper::DRIVER, 3, 4);
AccelStepper stepperY(AccelStepper::DRIVER, 5, 6);
MultiStepper steppers;

void setup() {
  Serial.begin(9600);
  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);
  stepperX.setMaxSpeed(500);
  stepperY.setMaxSpeed(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    parseCommand(command);
  }
  steppers.run();  // MUST be called every loop iteration to step the motor
}

void parseCommand(String command) {
  long xTarget = 0;
  long yTarget = 0;
  sscanf(command.c_str(), "X%ld Y%ld", &xTarget, &yTarget);
  long positions[2] = {xTarget, yTarget};
  steppers.moveTo(positions);  // absolute target position (steps); call ONCE, not every loop
}
