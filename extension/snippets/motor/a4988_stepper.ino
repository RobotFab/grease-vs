// snippet: a4988_stepper
// tags: motor, stepper, a4988, accelstepper, driver, nema17, cnc
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: NEMA17 stepper with A4988 using AccelStepper library
// prompts: A4988 stepper driver, NEMA17 stepper motor, AccelStepper A4988, CNC stepper direction driver A4988
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// A4988: STEP=3, DIR=4
AccelStepper stepper(AccelStepper::DRIVER, 3, 4);

void setup() {
  stepper.setMaxSpeed(800);
  stepper.setAcceleration(400);
  stepper.moveTo(200);   // 200 steps = 1 revolution (1/1 microstepping)
}

void loop() {
  if (stepper.distanceToGo() == 0) {
    stepper.moveTo(-stepper.currentPosition());  // reverse
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
}
