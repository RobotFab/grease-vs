// snippet: esp32_motor_175
// tags: esp32, motor, ledc, pwm, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: ESP32: AccelStepper on step=2 dir=3
// prompts: ESP32 stepper AccelStepper, ESP32 AccelStepper step dir, stepper motor ESP32, ESP32 NEMA17 AccelStepper, stepper position control ESP32
// verified: compile-only
// source: kd-generated (id 175)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// Define stepper motor connections and pins
#define STEP_PIN 2
#define DIR_PIN 3

// Create an instance of AccelStepper
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Global variables
int targetPosition = 0;
bool positionReached = false;

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(1000);       // steps/sec
  stepper.setAcceleration(500);    // steps/sec²
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    targetPosition = input.toInt();
    positionReached = false;
  }

  // Move stepper
  stepper.run();  // MUST be called every loop iteration to step the motor

  // Check if target position is reached
  if (stepper.distanceToGo() == 0 && !positionReached) {
    positionReached = true;
    Serial.println("DONE");
  }
}
