// snippet: stepper_accelstepper
// tags: stepper, accelstepper, acceleration, deceleration, ramp, nema17, a4988, drv8825
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Smooth stepper motion with acceleration/deceleration using AccelStepper
// prompts: stepper motor AccelStepper, NEMA17 stepper, smooth stepper acceleration, stepper ramp, A4988 stepper driver, DRV8825 stepper, stepper position control
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// DRIVER mode: use step + dir pins only (any A4988 / DRV8825 / TMC driver)
AccelStepper stepper(AccelStepper::DRIVER, 4, 5);  // step=4, dir=5

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(1000);       // steps/sec
  stepper.setAcceleration(500);    // steps/sec²
  stepper.moveTo(1600);            // 1 rev at 16 microsteps
}

void loop() {
  if (stepper.distanceToGo() == 0) {
    stepper.moveTo(-stepper.currentPosition());  // reverse
  }
  stepper.run();  // must be called as fast as possible — no delay!
}
