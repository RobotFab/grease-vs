// snippet: motor_stepper_341
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Using AccelStepper in DRIVER mode (step=3, dir=4), move 200 steps forward, wait 1 second, then 20…
// prompts: using, accelstepper, driver, (step=3, dir=4), steps
// verified: compile-only
// source: kd-generated (id 341)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// Stepper motor connections
const int STEP_PIN = 3;
const int DIR_PIN = 4;
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

const unsigned long WAIT_INTERVAL = 1000; // 1000ms (1 second) pause between moves

// State machine: FORWARD → WAIT → REVERSE → WAIT → FORWARD → ...
enum State { FORWARD, WAIT_AFTER_FORWARD, REVERSE, WAIT_AFTER_REVERSE };
State state = FORWARD;

unsigned long waitStartMs = 0;

void setup() {
  stepper.setMaxSpeed(200);     // 200 steps/sec
  stepper.setAcceleration(100); // 100 steps/sec^2
  stepper.moveTo(200);          // Move 200 steps forward
}

void loop() {
  switch (state) {
    case FORWARD:
      stepper.run();  // MUST call every iteration — steps toward +200
      if (stepper.distanceToGo() == 0) {
        // Arrived at +200 — start the wait timer
        waitStartMs = millis();
        state = WAIT_AFTER_FORWARD;
      }
      break;

    case WAIT_AFTER_FORWARD:
      stepper.run();  // keep calling run() even while parked (no-op when at target)
      if (millis() - waitStartMs >= WAIT_INTERVAL) {
        stepper.moveTo(-200);  // Move 200 steps back
        state = REVERSE;
      }
      break;

    case REVERSE:
      stepper.run();  // steps toward -200
      if (stepper.distanceToGo() == 0) {
        waitStartMs = millis();
        state = WAIT_AFTER_REVERSE;
      }
      break;

    case WAIT_AFTER_REVERSE:
      stepper.run();  // MUST be called every loop iteration to step the motor
      if (millis() - waitStartMs >= WAIT_INTERVAL) {
        stepper.moveTo(200);  // Move 200 steps forward again
        state = FORWARD;
      }
      break;
  }
}
