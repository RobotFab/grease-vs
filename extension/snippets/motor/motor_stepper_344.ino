// snippet: motor_stepper_344
// tags: motor, stepper, direction, accelstepper, multistepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, megaavr, romi32u4
// library: AccelStepper, MultiStepper
// description: Drive two AccelStepper motors (motor1: step=3,dir=4; motor2: step=5,dir=6) to the same absolute p…
// prompts: drive, accelstepper, motors, (motor1=3, dir=4
// verified: compile-only
// source: kd-generated (id 344)
// exact-include: AccelStepper.h, MultiStepper.h

#include <AccelStepper.h>
#include <MultiStepper.h>

AccelStepper motor1(AccelStepper::DRIVER, 3, 4);
AccelStepper motor2(AccelStepper::DRIVER, 5, 6);
MultiStepper steppers;

void setup() {
  motor1.setMaxSpeed(500);
  motor2.setMaxSpeed(500);
  steppers.addStepper(motor1);
  steppers.addStepper(motor2);
}

void loop() {
  static bool moved = false;
  if (!moved) {
    long positions[2] = {1000, 1000};
    steppers.moveTo(positions);  // absolute target position (steps); call ONCE, not every loop
    moved = true;
  }
  steppers.run();  // MUST be called every loop iteration to step the motor
}
