// snippet: active_mode_249
// tags: active, serial, command, response, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Active sketch: ZERO sets stepper position to 0, MOVE_REL <steps> moves relative, MOVE_ABS <steps> moves absolute
// prompts: active, sketch, stepper, position, move_rel, <steps>
// verified: compile-only
// source: kd-generated (id 249)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// DRIVER mode: use step + dir pins only (any A4988 / DRV8825 / TMC driver)
AccelStepper stepper(AccelStepper::DRIVER, 4, 5);  // step=4, dir=5

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(1000);       // steps/sec
  stepper.setAcceleration(500);    // steps/sec²
  Serial.println("Send: ZERO, MOVE_REL <steps>, MOVE_ABS <steps>, REPORT");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "ZERO") {
      // Reset the internal position counter to 0
      stepper.setCurrentPosition(0);
      Serial.println("Position zeroed");
    } else if (command.startsWith("MOVE_REL")) {
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        long steps = command.substring(spaceIdx + 1).toInt();
        stepper.move(steps);  // relative move
      }
    } else if (command.startsWith("MOVE_ABS")) {
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        long steps = command.substring(spaceIdx + 1).toInt();
        stepper.moveTo(steps);  // absolute move
      }
    } else if (command == "REPORT") {
      Serial.print("Current Position: ");
      Serial.println(stepper.currentPosition());  // current position in steps (no currentSpeed() method exists)
    }
  }

  stepper.run();  // must be called as fast as possible — no delay!
}
