// snippet: motor_stepper_342
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Set an AccelStepper (step=3, dir=4) to a constant speed of 300 steps/sec using setSpeed()
// prompts: accelstepper, (step=3, dir=4), constant, speed, steps/sec
// verified: compile-only
// source: kd-generated (id 342)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// Stepper motor connections
const int STEP_PIN = 3;
const int DIR_PIN = 4;
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

unsigned long prevMs = 0;
const unsigned long PRINT_INTERVAL = 1000; // 1000ms (1 second)

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(300);  // 300 steps/sec
  stepper.setSpeed(300);     // Set constant speed
}

void loop() {
  unsigned long now = millis();

  // Run the stepper at the set speed
  stepper.runSpeed();  // steps at constant speed; call every loop iteration

  // Print current position every second
  if (now - prevMs >= PRINT_INTERVAL) {
    prevMs = now;
    Serial.print("Current Position: ");
    Serial.println(stepper.currentPosition());  // current position in steps (no currentSpeed() method exists)
  }
}
