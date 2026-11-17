// snippet: motor_stepper_350
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Home sequence for AccelStepper (step=3, dir=4): move at -150 steps/s until limit switch on pin 8 …
// prompts: sequence, accelstepper, (step=3, dir=4), steps/s, until
// verified: compile-only
// source: kd-generated (id 350)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int LIMIT_SWITCH_PIN = 8;

// Non-blocking homing state machine — avoids blocking the loop while seeking.
enum HomeState { IDLE, SEEKING, BACKING_OFF, HOMED };
HomeState homeState = IDLE;

unsigned long homeTimeoutMs = 0;
const unsigned long HOME_TIMEOUT = 5000;  // give up if endstop never triggers

void setup() {
  Serial.begin(9600);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  stepper.setMaxSpeed(150);
  stepper.setAcceleration(100);
}

void startHoming() {
  // Issue a large relative move toward the switch; run() will step until
  // the switch triggers or the timeout expires.
  stepper.move(-100000);  // relative move (steps added to current pos); call ONCE
  homeTimeoutMs = millis();
  homeState = SEEKING;
  Serial.println("Homing...");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "HOME" && homeState == IDLE) {
      startHoming();
    }
  }

  switch (homeState) {
    case IDLE:
    case HOMED:
      break;

    case SEEKING:
      stepper.run();  // step toward the limit switch
      if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
        // Switch triggered — stop and back off 50 steps
        stepper.stop();  // decelerate to a halt at current position
        stepper.move(50);  // back off 50 steps away from the switch
        homeState = BACKING_OFF;
      } else if (millis() - homeTimeoutMs > HOME_TIMEOUT) {
        Serial.println("HOME timeout");
        stepper.stop();
        homeState = IDLE;
      }
      break;

    case BACKING_OFF:
      stepper.run();  // step through the 50-step backoff
      if (stepper.distanceToGo() == 0) {
        // Backoff complete — set this as position 0
        stepper.setCurrentPosition(0);
        Serial.println("HOMED");
        homeState = HOMED;
      }
      break;
  }

  // Allow manual jogging when not homing
  if (homeState == IDLE || homeState == HOMED) {
    stepper.run();  // MUST be called every loop iteration to step the motor
  }
}
