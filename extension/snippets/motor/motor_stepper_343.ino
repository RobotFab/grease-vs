// snippet: motor_stepper_343
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Using AccelStepper (step=3, dir=4), move to absolute position 1000, then to 0, then to 500
// prompts: using, accelstepper, (step=3, dir=4), absolute, position
// verified: compile-only
// source: kd-generated (id 343)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);

// Sequence of absolute positions to visit, in order.
// The description promises: 1000 → 0 → 500, then repeat.
const long positions[] = {1000, 0, 500};
const int NUM_POSITIONS = 3;
int seqIndex = 0;

void setup() {
  stepper.setMaxSpeed(600);
  stepper.setAcceleration(400);
  // Start the first move
  stepper.moveTo(positions[seqIndex]);  // absolute target position (steps); call ONCE, not every loop
}

void loop() {
  stepper.run();  // MUST call every iteration

  // When the current target is reached, advance to the next position
  if (stepper.distanceToGo() == 0) {
    seqIndex = (seqIndex + 1) % NUM_POSITIONS;  // wrap around
    stepper.moveTo(positions[seqIndex]);  // absolute target position (steps); call ONCE, not every loop
  }
}
