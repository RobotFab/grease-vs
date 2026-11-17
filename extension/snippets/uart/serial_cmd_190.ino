// snippet: serial_cmd_190
// tags: serial, command, parser, uart, accelstepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Serial watchdog: send PING every 2 seconds from master
// prompts: watchdog, seconds, master, received, seconds, motor
// verified: compile-only
// source: kd-generated (id 190)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);
unsigned long lastPingMs = 0;
const unsigned long PING_INTERVAL = 2000;
const unsigned long WATCHDOG_TIMEOUT = 5000;

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(500.0);
  stepper.setAcceleration(500);
}

void loop() {
  unsigned long now = millis();

  // Send PING every 2 seconds
  if (now - lastPingMs >= PING_INTERVAL) {
    lastPingMs = now;
    Serial.println("PING");
  }

  // Check for PONG from master
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "PONG") {
      lastPingMs = now;
    }
  }

  // Stop motor if no PONG received for 5 seconds
  if (now - lastPingMs >= WATCHDOG_TIMEOUT) {
    stepper.stop();
    Serial.println("WATCHDOG");
  }

  // Run the stepper motor
  stepper.run();  // MUST be called every loop iteration to step the motor
}
