// snippet: multi_peripheral_536
// tags: accelstepper, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: AccelStepper, Servo
// description: CNC plotter: two AccelStepper motors (X: step=3,dir=4; Y: step=5,dir=6) plus a servo pen-lift on …
// prompts: plotter, accelstepper, motors, step=3, dir=4, step=5
// verified: compile-only
// source: kd-generated (id 536)
// exact-include: AccelStepper.h, Servo.h

#include <AccelStepper.h>
#include <Servo.h>

// Stepper motor pins
const int X_STEP_PIN = 3;
const int X_DIR_PIN = 4;
const int Y_STEP_PIN = 5;
const int Y_DIR_PIN = 6;

// Servo pin
const int SERVO_PIN = 9;

// Limit switch pins
const int X_LIMIT_PIN = 2;
const int Y_LIMIT_PIN = 7;

AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

Servo penServo;

bool penDown = false;

void setup() {
  Serial.begin(9600);
  pinMode(X_LIMIT_PIN, INPUT_PULLUP);
  pinMode(Y_LIMIT_PIN, INPUT_PULLUP);
  penServo.attach(SERVO_PIN);
  penServo.write(90);  // Start with pen up

  homeMotors();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    parseCommand(command);
  }
  stepperX.run();  // MUST be called every loop iteration to step the motor
  stepperY.run();  // MUST be called every loop iteration to step the motor
}

void homeMotors() {
  const unsigned long HOME_TIMEOUT = 5000;  // give up if the endstop never triggers

  unsigned long tX = millis();
  while (digitalRead(X_LIMIT_PIN) == HIGH) {
    stepperX.move(-1000);  // relative move (steps added to current pos); call ONCE
    stepperX.run();  // MUST be called every loop iteration to step the motor
    if (millis() - tX > HOME_TIMEOUT) {  // don't hang forever on a bad/disconnected switch
      Serial.println("X homing timeout");
      break;
    }
  }
  stepperX.setCurrentPosition(0);

  unsigned long tY = millis();
  while (digitalRead(Y_LIMIT_PIN) == HIGH) {
    stepperY.move(-1000);  // relative move (steps added to current pos); call ONCE
    stepperY.run();  // MUST be called every loop iteration to step the motor
    if (millis() - tY > HOME_TIMEOUT) {
      Serial.println("Y homing timeout");
      break;
    }
  }
  stepperY.setCurrentPosition(0);

  Serial.println("Motors homed.");
}

void parseCommand(String command) {
  if (command.startsWith("G0")) {
    int x = command.indexOf('X') + 1;
    int y = command.indexOf('Y') + 1;
    int xVal = command.substring(x).toInt();
    int yVal = command.substring(y).toInt();
    moveXY(xVal, yVal);
  } else if (command.startsWith("M3")) {
    penDown = true;
    penServo.write(0);  // Lower pen
  } else if (command.startsWith("M5")) {
    penDown = false;
    penServo.write(90);  // Raise pen
  }
}

void moveXY(int x, int y) {
  stepperX.moveTo(x);  // absolute target position (steps); call ONCE, not every loop
  stepperY.moveTo(y);  // absolute target position (steps); call ONCE, not every loop
}
