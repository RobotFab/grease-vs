// snippet: motor_stepper_354
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Implement a stepper state machine with states IDLE, HOMING, RUNNING, FAULT using an enum
// prompts: implement, stepper, state, machine, states, homing
// verified: compile-only
// source: kd-generated (id 354)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

enum State { IDLE, HOMING, RUNNING, FAULT };
State state = IDLE;
AccelStepper stepper(AccelStepper::DRIVER, 2, 3);
const int LIMIT_SWITCH_PIN = 4;
const int BUTTON_PIN = 8;
const int MOTOR_SPEED = 300;
const long RUN_TARGET = 5000;  // position to move to after homing

bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

void setup() {
  Serial.begin(9600);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  stepper.setMaxSpeed(MOTOR_SPEED);
  stepper.setAcceleration(MOTOR_SPEED);
}

void loop() {
  unsigned long now = millis();

  // Button debouncing
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = now;
  }
  if ((now - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != lastButtonState) {
      lastButtonState = reading;
      if (lastButtonState == LOW) {  // Button press
        if (state == IDLE) {
          // Start homing: issue a large move toward the limit switch
          stepper.move(-100000);  // relative move (steps added to current pos); call ONCE
          state = HOMING;
          Serial.println("HOMING");
        }
      }
    }
  }

  // State machine logic
  switch (state) {
    case IDLE:
      break;

    case HOMING:
      // Must call run() here so the stepper actually moves toward the switch.
      stepper.run();  // MUST be called every loop iteration to step the motor
      if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
        // Switch triggered — stop, set home, transition to RUNNING
        stepper.stop();
        stepper.setCurrentPosition(0);
        stepper.moveTo(RUN_TARGET);  // begin the production move
        state = RUNNING;
        Serial.println("RUNNING");
      }
      break;

    case RUNNING:
      stepper.run();  // MUST be called every loop iteration to step the motor
      if (stepper.distanceToGo() == 0) {
        // Reached the run target — return to IDLE
        state = IDLE;
        Serial.println("IDLE");
      }
      break;

    case FAULT:
      // Stepper stopped — do not call run()
      break;
  }
}
