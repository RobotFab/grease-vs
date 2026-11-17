// snippet: motor_stepper_360
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Move an AccelStepper (step=3, dir=4) through a sequence of 5 positions stored in an array: {0, 50…
// prompts: accelstepper, (step=3, dir=4), through, sequence, positions
// verified: compile-only
// source: kd-generated (id 360)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// AccelStepper setup
AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int positions[] = {0, 500, 1000, 750, 200};
const int numPositions = sizeof(positions) / sizeof(positions[0]);
int currentPos = 0;
unsigned long dwellStart = 0;
const unsigned long dwellTime = 1000; // 1 second

void setup() {
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  stepper.moveTo(positions[currentPos]);  // absolute target position (steps); call ONCE, not every loop
}

void loop() {
  if (stepper.distanceToGo() == 0) {
    stepper.moveTo(positions[currentPos]);  // absolute target position (steps); call ONCE, not every loop
  }
  stepper.run();  // MUST be called every loop iteration to step the motor

  if (stepper.distanceToGo() == 0 && millis() - dwellStart >= dwellTime) {
    currentPos = (currentPos + 1) % numPositions;
    dwellStart = millis();
  }
}
