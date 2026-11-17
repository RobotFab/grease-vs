// snippet: pico_210
// tags: pico, rp2040, raspberry-pi, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: RPi Pico: AccelStepper on step=2 dir=3
// prompts: stepper motor Pico, AccelStepper RP2040, stepper Raspberry Pi Pico, NEMA17 Pi Pico, step dir motor RP2040, AccelStepper Pi Pico
// verified: compile-only
// source: kd-generated (id 210)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(1600);
  stepper.setAcceleration(800);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("GOTO")) {
      int position = command.substring(5).toInt();
      stepper.moveTo(position);  // absolute target position (steps); call ONCE, not every loop
    }
  }

  stepper.run();  // MUST be called every loop iteration to step the motor

  if (stepper.distanceToGo() == 0) {
    Serial.println("DONE");
  }
}
