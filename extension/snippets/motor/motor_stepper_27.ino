// snippet: motor_stepper_27
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Stepper jog mode: AccelStepper on pins 2/3
// prompts: stepper, accelstepper, steps, steps, print, current
// verified: compile-only
// source: kd-generated (id 27)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);  // step=2, dir=3

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(1000);       // steps/sec
  stepper.setAcceleration(500);    // steps/sec²
  stepper.moveTo(0);               // start at position 0
}

void loop() {
  if (Serial.available() > 0) {
    int jog = Serial.parseInt();
    if (jog == 1) {
      stepper.moveTo(stepper.currentPosition() + 100);  // absolute target position (steps); call ONCE, not every loop
    } else if (jog == -1) {
      stepper.moveTo(stepper.currentPosition() - 100);  // absolute target position (steps); call ONCE, not every loop
    }
  }

  if (stepper.distanceToGo() == 0) {
    stepper.runToPosition();
    Serial.print("Current Position: ");
    Serial.println(stepper.currentPosition());  // current position in steps (no currentSpeed() method exists)
  }

  stepper.run();  // MUST be called every loop iteration to step the motor
}
