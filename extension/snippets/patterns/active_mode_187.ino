// snippet: active_mode_187
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: RAMP <target> <ms> ramps motor PWM on pin 5 from current to target over given milliseconds
// prompts: active, sketch, <target>, ramps, motor, current
// verified: compile-only
// source: kd-generated (id 187)
// exact-include: Arduino.h

#include <Arduino.h>

const int MOTOR_PIN = 5;

float currentSpeed = 0.0;
float startSpeed = 0.0;
float targetSpeed = 0.0;
unsigned long rampStart = 0;
unsigned long rampDuration = 0;
bool ramping = false;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Send: RAMP <target> <ms>");
}

void loop() {
  unsigned long now = millis();

  // Handle serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("RAMP")) {
      // Parse: RAMP <target> <ms>
      int firstSpace = command.indexOf(' ');
      int secondSpace = command.indexOf(' ', firstSpace + 1);
      if (firstSpace > 0 && secondSpace > 0) {
        int target = command.substring(firstSpace + 1, secondSpace).toInt();
        unsigned long ms = command.substring(secondSpace + 1).toInt();
        target = constrain(target, 0, 255);

        startSpeed = currentSpeed;
        targetSpeed = (float)target;
        rampDuration = ms;
        rampStart = now;
        ramping = true;
      }
    }
  }

  // Linear ramp interpolation
  if (ramping) {
    if (now - rampStart < rampDuration) {
      float progress = (float)(now - rampStart) / (float)rampDuration;
      currentSpeed = startSpeed + (targetSpeed - startSpeed) * progress;
    } else {
      currentSpeed = targetSpeed;
      ramping = false;
    }
    analogWrite(MOTOR_PIN, (int)currentSpeed);
  }

  // Report speed every 100ms
  static unsigned long lastPrintMs = 0;
  if (now - lastPrintMs >= 100) {
    lastPrintMs = now;
    Serial.print("Speed: ");
    Serial.println((int)currentSpeed);
  }
}
