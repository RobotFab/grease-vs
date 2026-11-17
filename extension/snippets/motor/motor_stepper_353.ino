// snippet: motor_stepper_353
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Parse minimal G-code from Serial: G0 X<n> moves the X-axis AccelStepper (step=3,dir=4) to positio…
// prompts: parse, minimal, g-code, moves, x-axis, accelstepper
// verified: compile-only
// source: kd-generated (id 353)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepperX(AccelStepper::DRIVER, 3, 4);
AccelStepper stepperY(AccelStepper::DRIVER, 5, 6);

void setup() {
  Serial.begin(9600);
  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(500);
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(500);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    parseGCode(command);
  }
}

void parseGCode(String command) {
  if (command.startsWith("G0 X")) {
    int targetX = command.substring(4).toInt();
    stepperX.moveTo(targetX);  // absolute target position (steps); call ONCE, not every loop
    while (stepperX.distanceToGo() != 0) {
      stepperX.run();  // MUST be called every loop iteration to step the motor
    }
    Serial.println("OK");
  } else if (command.startsWith("G0 Y")) {
    int targetY = command.substring(4).toInt();
    stepperY.moveTo(targetY);  // absolute target position (steps); call ONCE, not every loop
    while (stepperY.distanceToGo() != 0) {
      stepperY.run();  // MUST be called every loop iteration to step the motor
    }
    Serial.println("OK");
  }
}
