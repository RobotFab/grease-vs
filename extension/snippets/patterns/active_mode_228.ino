// snippet: active_mode_228
// tags: active, serial, command, response, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Active sketch: HOME command runs stepper to limit switch on pin 4, sets position 0
// prompts: active, sketch, command, stepper, limit, switch
// verified: compile-only
// source: kd-generated (id 228)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

#define LIMIT_SWITCH_PIN 4
#define STEP_PIN 2
#define DIR_PIN 3

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
int limitSwitchState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("HOME")) {
      unsigned long tHome = millis();
      while (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
        stepper.move(-1);  // relative move (steps added to current pos); call ONCE
        if (millis() - tHome > 5000) {  // give up if the endstop never triggers
          Serial.println("HOME timeout");
          break;
        }
      }
      stepper.setCurrentPosition(0);
      Serial.println("HOME complete");
    } else if (command.startsWith("GOTO")) {
      int steps = command.substring(5).toInt();
      stepper.moveTo(steps);  // absolute target position (steps); call ONCE, not every loop
      while (stepper.distanceToGo() != 0) {
        stepper.run();  // MUST be called every loop iteration to step the motor
      }
      Serial.println("GOTO complete");
    } else if (command.startsWith("POSITION")) {
      Serial.print("Current position: ");
      Serial.println(stepper.currentPosition());  // current position in steps (no currentSpeed() method exists)
    }
  }
}
