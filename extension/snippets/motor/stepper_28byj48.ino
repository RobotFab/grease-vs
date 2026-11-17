// snippet: stepper_28byj48
// tags: stepper, 28byj48, uln2003, motor, 4wire, steps, accelstepper, full4wire
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Drive a 28BYJ-48 stepper motor through a ULN2003 driver board using AccelStepper
//   in FULL4WIRE mode. The 28BYJ-48 has 2048 steps/revolution at 1:64 gear ratio.
//   Wiring: IN1→PIN1, IN2→PIN2, IN3→PIN3, IN4→PIN4, driver VCC→5V, GND→GND.
//   AccelStepper pin order for 28BYJ-48: (FULL4WIRE, IN1, IN3, IN2, IN4) — interleaved pairs.
//   API: setMaxSpeed(steps/s), setAcceleration(steps/s²), moveTo(abs), move(rel), run() in loop.
//   run() is NON-BLOCKING — call every loop iteration. Do NOT use step() or setSpeed() (those are
//   the old Stepper library; AccelStepper uses setMaxSpeed + setAcceleration + moveTo + run).
// prompts: 28BYJ-48 stepper motor, ULN2003 driver, small 5V stepper, gear stepper motor, 4-wire stepper 2048 steps, 28byj48 AccelStepper
// exact-include: AccelStepper.h

#include <AccelStepper.h>

// 28BYJ-48 wiring: IN1=8, IN2=9, IN3=10, IN4=11
// Pin order for AccelStepper FULL4WIRE: (IN1, IN3, IN2, IN4) — pairs interleaved
AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 10, 9, 11);

#define STEPS_PER_REV  2048

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(500);       // steps/second
  stepper.setAcceleration(200);   // steps/second²
  Serial.println("28BYJ-48 ready (AccelStepper)");
}

void loop() {
  // moveTo() sets absolute target; run() executes one step if needed (non-blocking)
  if (stepper.distanceToGo() == 0) {
    // Alternate CW / CCW one full revolution
    static int dir = 1;
    stepper.moveTo(stepper.currentPosition() + dir * STEPS_PER_REV);  // absolute target position (steps); call ONCE, not every loop
    dir = -dir;
    Serial.println(dir > 0 ? "CCW one revolution" : "CW one revolution");
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
}
