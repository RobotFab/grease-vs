// snippet: motor_stepper_348
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Map a potentiometer on A0 to stepper speed: 0 V = 0 steps/s, 5 V = 600 steps/s
// prompts: potentiometer, stepper, speed, steps/s, steps/s, apply
// verified: compile-only
// source: kd-generated (id 348)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int POT_PIN = A0;
const unsigned long UPDATE_INTERVAL = 100;

void setup() {
  stepper.setMaxSpeed(600);
  stepper.setAcceleration(600);
}

void loop() {
  static unsigned long lastUpdateMs = 0;
  unsigned long now = millis();

  if (now - lastUpdateMs >= UPDATE_INTERVAL) {
    lastUpdateMs = now;
    int potValue = analogRead(POT_PIN);
    float speed = map(potValue, 0, 1023, 0, 600);
    stepper.setSpeed(speed);
  }

  stepper.runSpeed();  // steps at constant speed; call every loop iteration
}
