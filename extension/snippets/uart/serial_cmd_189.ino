// snippet: serial_cmd_189
// tags: serial, command, parser, uart, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Serial command with two arguments: MOVE <steps> <speed>
// prompts: command, arguments, <steps>, <speed>, accelstepper, steps
// verified: compile-only
// source: kd-generated (id 189)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);
float moveSpeed = 500.0;

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(moveSpeed);
  stepper.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("MOVE")) {
      // Parse "MOVE <steps> <speed>" — sscanf handles variable-width numbers
      long steps = 0;
      float speed = 0;
      if (sscanf(command.c_str() + 4, " %ld %f", &steps, &speed) >= 1) {
        if (speed > 0) {
          moveSpeed = speed;
          stepper.setMaxSpeed(moveSpeed);
        }
        stepper.moveTo(steps);  // absolute target position (steps); call ONCE, not every loop
        // Do NOT block with while(distanceToGo()) — that defeats AccelStepper's
        // non-blocking design. run() below will step toward the target each loop.
      }
    }
  }

  // Non-blocking: call run() every iteration so the stepper steps toward
  // its target while the loop stays responsive to new serial commands.
  stepper.run();  // MUST be called every loop iteration to step the motor
}
