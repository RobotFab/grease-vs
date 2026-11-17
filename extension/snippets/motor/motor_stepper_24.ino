// snippet: motor_stepper_24
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Stepper homing: AccelStepper on step=2, dir=3
// prompts: stepper, homing, accelstepper=2, dir=3, limit
// verified: compile-only
// source: kd-generated (id 24)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);
const int LIMIT_SWITCH_PIN = 4;

void setup() {
  Serial.begin(9600);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "HOME") {
      unsigned long tHome = millis();
      while (digitalRead(LIMIT_SWITCH_PIN) == HIGH) {
        stepper.move(-1000);  // relative move (steps added to current pos); call ONCE
        stepper.run();  // MUST be called every loop iteration to step the motor
        if (millis() - tHome > 5000) {  // give up if the endstop never triggers
          Serial.println("HOME timeout");
          break;
        }
      }
      stepper.setCurrentPosition(0);
      Serial.println("HOMED");
    }
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
}
