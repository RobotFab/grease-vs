// snippet: active_mode_101
// tags: active, serial, command, response, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Active sketch: STEP <n> <dir> moves AccelStepper on pins 2/3
// prompts: active, sketch, <dir>, moves, accelstepper, reply
// verified: compile-only
// source: kd-generated (id 101)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 2);  // STEP=3, DIR=2

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(1600);       // steps/sec (1/16 microstepping → 3200 steps/rev)
  stepper.setAcceleration(800);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("STEP")) {
      int pos = command.substring(5).toInt();
      stepper.moveTo(pos);  // absolute target position (steps); call ONCE, not every loop
      Serial.println("MOVING");
    }
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
  if (stepper.distanceToGo() == 0) {
    Serial.println("DONE");
  }
}
