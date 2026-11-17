// snippet: motor_stepper_356
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Enable the SLEEP pin (pin 10, active LOW) on a DRV8825 to save power: assert it LOW when the step…
// prompts: enable, sleep, active, drv8825, power, assert
// verified: compile-only
// source: kd-generated (id 356)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int SLEEP_PIN = 10;
const unsigned long SLEEP_TIMEOUT = 5000;  // 5 seconds

void setup() {
  pinMode(SLEEP_PIN, OUTPUT);
  digitalWrite(SLEEP_PIN, LOW);  // Enable sleep initially
  stepper.setMaxSpeed(1600);
  stepper.setAcceleration(800);
}

void loop() {
  static unsigned long lastMoveTime = 0;
  static bool isStepperActive = false;

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("STEP")) {
      int pos = command.substring(5).toInt();
      stepper.moveTo(pos);  // absolute target position (steps); call ONCE, not every loop
      isStepperActive = true;
      lastMoveTime = millis();
      digitalWrite(SLEEP_PIN, HIGH);  // Disable sleep
    }
  }

  if (isStepperActive && millis() - lastMoveTime >= SLEEP_TIMEOUT) {
    isStepperActive = false;
    digitalWrite(SLEEP_PIN, LOW);  // Enable sleep
  }

  stepper.run();  // MUST be called every loop iteration to step the motor
}
