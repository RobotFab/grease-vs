// snippet: drv8825_stepper
// tags: motor, stepper, drv8825, accelstepper, driver, nema17, 1_32 microstepping
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: NEMA17 stepper with DRV8825 using AccelStepper, non-blocking
// prompts: DRV8825 stepper driver, NEMA17 DRV8825, AccelStepper DRV8825, microstepping stepper, 1/32 step mode driver
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// DRV8825: STEP=3, DIR=4, ENABLE=2 (active LOW)
const int ENABLE_PIN = 2;
AccelStepper stepper(AccelStepper::DRIVER, 3, 4);

void setup() {
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);  // enable driver
  stepper.setMaxSpeed(1600);       // steps/sec (1/16 microstepping → 3200 steps/rev)
  stepper.setAcceleration(800);
  stepper.moveTo(3200);  // absolute target position (steps); call ONCE, not every loop
}

void loop() {
  if (stepper.distanceToGo() == 0) {
    stepper.moveTo(-stepper.currentPosition());  // absolute target position (steps); call ONCE, not every loop
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
}
