// snippet: motor_stepper_347
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Jog an AccelStepper (step=3, dir=4): hold button on pin 5 (INPUT_PULLUP) to jog CW, hold button o…
// prompts: accelstepper, (step=3, dir=4), button, (input_pullup), button
// verified: compile-only
// source: kd-generated (id 347)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int CW_BUTTON_PIN = 5;
const int CCW_BUTTON_PIN = 6;
const int DEBOUNCE_MS = 50;

int lastCWButtonState = HIGH;
int lastCCWButtonState = HIGH;
unsigned long lastCWChangeMs = 0;
unsigned long lastCCWChangeMs = 0;

void setup() {
  pinMode(CW_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CCW_BUTTON_PIN, INPUT_PULLUP);
  stepper.setMaxSpeed(200);
  stepper.setAcceleration(200);
}

void loop() {
  int CWButtonState = digitalRead(CW_BUTTON_PIN);
  int CCWButtonState = digitalRead(CCW_BUTTON_PIN);

  if (CWButtonState != lastCWButtonState) lastCWChangeMs = millis();
  if (CCWButtonState != lastCCWButtonState) lastCCWChangeMs = millis();

  if (millis() - lastCWChangeMs > DEBOUNCE_MS) {
    if (CWButtonState == LOW) {
      stepper.move(1);  // relative move (steps added to current pos); call ONCE
    }
  }

  if (millis() - lastCCWChangeMs > DEBOUNCE_MS) {
    if (CCWButtonState == LOW) {
      stepper.move(-1);  // relative move (steps added to current pos); call ONCE
    }
  }

  stepper.run();  // MUST be called every loop iteration to step the motor
}
