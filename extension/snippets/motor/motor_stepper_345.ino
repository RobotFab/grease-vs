// snippet: motor_stepper_345
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Home an AccelStepper (step=3, dir=4) to a limit switch on pin 8 (active LOW, INPUT_PULLUP)
// prompts: accelstepper, (step=3, dir=4), limit, switch, (active
// verified: compile-only
// source: kd-generated (id 345)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int LIMIT_SWITCH_PIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  stepper.setMaxSpeed(100);
  stepper.setAcceleration(50);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "HOME") {
      unsigned long tHome = millis();
      while (digitalRead(LIMIT_SWITCH_PIN) == HIGH) {
        stepper.move(-100);  // relative move (steps added to current pos); call ONCE
        stepper.run();  // MUST be called every loop iteration to step the motor
        if (millis() - tHome > 5000) {  // give up if the endstop never triggers
          Serial.println("HOME timeout");
          break;
        }
      }
      stepper.setCurrentPosition(0);
      Serial.println("HOMED");
    } else if (command == "MOVE") {
      stepper.moveTo(500);  // absolute target position (steps); call ONCE, not every loop
      while (stepper.distanceToGo() != 0) {
        stepper.run();  // MUST be called every loop iteration to step the motor
      }
      Serial.println("MOVED TO 500");
    }
  }
}
