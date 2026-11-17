// snippet: motor_stepper_355
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Ramp AccelStepper (step=3, dir=4) from 0 to 800 steps/s over 3 seconds, hold for 2 seconds, then …
// prompts: accelstepper, (step=3, dir=4), steps/s, seconds, seconds
// verified: compile-only
// source: kd-generated (id 355)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);

const unsigned long RAMP_TIME = 3000;      // 3 seconds to reach target speed
const unsigned long HOLD_TIME = 2000;      // 2 seconds at full speed
const unsigned long PRINT_INTERVAL = 200;  // 200 ms status print
const int TARGET_SPEED = 800;              // steps/sec

// Acceleration so the ramp from 0 to 800 steps/s takes exactly 3 seconds:
// accel = Δspeed / Δtime = 800 / 3.0 ≈ 266.67 steps/s²
const float RAMP_ACCEL = TARGET_SPEED / (RAMP_TIME / 1000.0);

// State machine: RAMP_UP → HOLD → RAMP_DOWN → DONE
enum RampState { RAMP_UP, HOLD, RAMP_DOWN, DONE };
RampState state = RAMP_UP;

unsigned long stateStartMs = 0;
unsigned long lastPrintMs = 0;

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(TARGET_SPEED);
  stepper.setAcceleration(RAMP_ACCEL);

  // Issue a very large move so the motor accelerates to maxSpeed and
  // keeps running — we control the phase transitions via timers.
  stepper.move(1000000L);  // relative move (steps added to current pos); call ONCE
  stateStartMs = millis();
  Serial.print("RAMP_UP (accel=");
  Serial.print(RAMP_ACCEL);
  Serial.println(" steps/s^2)");
}

void loop() {
  unsigned long now = millis();
  stepper.run();  // MUST call every iteration

  switch (state) {
    case RAMP_UP:
      // After 3s the motor has reached TARGET_SPEED — transition to HOLD
      if (now - stateStartMs >= RAMP_TIME) {
        state = HOLD;
        stateStartMs = now;
        Serial.println("HOLD");
      }
      break;

    case HOLD:
      // Hold at full speed for 2s, then decelerate
      if (now - stateStartMs >= HOLD_TIME) {
        // stop() decelerates at the same acceleration rate — ~3s to halt
        stepper.stop();
        state = RAMP_DOWN;
        stateStartMs = now;
        Serial.println("RAMP_DOWN");
      }
      break;

    case RAMP_DOWN:
      // Wait until the motor has fully stopped
      if (stepper.distanceToGo() == 0) {
        state = DONE;
        Serial.println("DONE");
      }
      break;

    case DONE:
      // Motor stopped — nothing more to do
      break;
  }

  // Periodic speed print (uses separate timer so it never starves)
  if (now - lastPrintMs >= PRINT_INTERVAL) {
    lastPrintMs = now;
    Serial.print("Current Speed: ");
    Serial.print(stepper.speed());
    Serial.print(" steps/s  State: ");
    Serial.println(state);
  }
}
