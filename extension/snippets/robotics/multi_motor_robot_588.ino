// snippet: multi_motor_robot_588
// tags: robot, multi-motor, differential-drive, millis, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Tank with arm: differential drive (L298N ENA=5,IN1=7,IN2=8,ENB=6,IN3=4,IN4=2) plus 2-DOF arm (shoulder servo pin 9, elbow servo pin 10)
// prompts: differential, drive, (l298n, 5–10), 2-dof, (shoulder
// verified: compile-only
// source: kd-generated (id 588)
// exact-include: Servo.h

#include <Servo.h>

// Motor pins (L298N). Enable pins are PWM (Timer0 on AVR); direction pins are plain digital.
const int ENA = 5;   // left speed  (PWM)
const int IN1 = 7;   // left direction
const int IN2 = 8;
const int ENB = 6;   // right speed (PWM)
const int IN3 = 4;   // right direction
const int IN4 = 2;

// Servo pins. On AVR the Servo library takes over Timer1 (pins 9/10), so those are
// reserved for the servos and kept clear of the motor PWM pins above.
const int SHOULDER_PIN = 9;
const int ELBOW_PIN = 10;

Servo shoulder;
Servo elbow;

int driveLeft = 0;
int driveRight = 0;
int shoulderAngle = 0;
int elbowAngle = 0;

void setup() {
  Serial.begin(9600);

  // Motor setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Servo setup
  shoulder.attach(SHOULDER_PIN);
  elbow.attach(ELBOW_PIN);

  // Initialize motor and servo states
  driveLeft = 0;
  driveRight = 0;
  shoulderAngle = 0;
  elbowAngle = 0;

  // Set initial motor and servo positions
  analogWrite(ENA, driveLeft);
  analogWrite(ENB, driveRight);
  shoulder.write(shoulderAngle);
  elbow.write(elbowAngle);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("D")) {
      parseDriveCommand(command);
    } else if (command.startsWith("A")) {
      parseArmCommand(command);
    }
  }

  // Update motor and servo states
  updateMotors();
  updateServos();
}

void parseDriveCommand(String command) {
  int commaIndex = command.indexOf(',');
  if (commaIndex != -1) {
    driveLeft = command.substring(1, commaIndex).toInt();
    driveRight = command.substring(commaIndex + 1).toInt();
  }
}

void parseArmCommand(String command) {
  int commaIndex = command.indexOf(',');
  if (commaIndex != -1) {
    shoulderAngle = command.substring(1, commaIndex).toInt();
    elbowAngle = command.substring(commaIndex + 1).toInt();
  }
}

void updateMotors() {
  // Left motor: sign of driveLeft selects direction, magnitude sets speed.
  digitalWrite(IN1, driveLeft >= 0 ? HIGH : LOW);
  digitalWrite(IN2, driveLeft >= 0 ? LOW : HIGH);
  analogWrite(ENA, constrain(abs(driveLeft), 0, 255));

  // Right motor.
  digitalWrite(IN3, driveRight >= 0 ? HIGH : LOW);
  digitalWrite(IN4, driveRight >= 0 ? LOW : HIGH);
  analogWrite(ENB, constrain(abs(driveRight), 0, 255));
}

void updateServos() {
  shoulder.write(shoulderAngle);
  elbow.write(elbowAngle);
}
