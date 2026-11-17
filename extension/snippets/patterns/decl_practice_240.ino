// snippet: decl_practice_240
// tags: globals, declare, millis, nonblocking, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Declare AccelStepper stepper(AccelStepper::DRIVER, 2, 3) at global scope
// prompts: declare, accelstepper, stepper(accelstepper, driver, scope, setmaxspeed(1000)
// verified: compile-only
// source: kd-generated (id 240)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);
int targetPosition = 0;

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    targetPosition = Serial.parseInt();
    stepper.moveTo(targetPosition);  // absolute target position (steps); call ONCE, not every loop
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
}
