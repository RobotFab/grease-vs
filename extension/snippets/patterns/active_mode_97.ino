// snippet: active_mode_97
// tags: active, serial, command, response, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: Active sketch: MOTOR_FWD <speed> and MOTOR_REV <speed> over serial drive L298N on pins 5/6/7
// prompts: active, sketch, motor_fwd, <speed>, motor_rev, <speed>
// verified: compile-only
// source: kd-generated (id 97)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

int motorSpeed = 0;
int motorDir = 0;

// SoftwareSerial on pins 10 (RX) / 11 (TX) — AVR-only library
SoftwareSerial Serial2(10, 11);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(5, OUTPUT); // ENA
  pinMode(6, OUTPUT); // IN1
  pinMode(7, OUTPUT); // IN2
}

void loop() {
  // Read commands from SoftwareSerial (Serial2)
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();

    if (command.startsWith("MOTOR_FWD")) {
      // Parse speed after the command + space
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        motorSpeed = command.substring(spaceIdx + 1).toInt();
        motorSpeed = constrain(motorSpeed, 0, 255);
        motorDir = 1;
        motorForward(motorSpeed);
      }
    } else if (command.startsWith("MOTOR_REV")) {
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        motorSpeed = command.substring(spaceIdx + 1).toInt();
        motorSpeed = constrain(motorSpeed, 0, 255);
        motorDir = -1;
        motorBackward(motorSpeed);
      }
    }
  }

  // Print status only when a command changes state (avoid flooding)
  static int lastDir = 0, lastSpeed = 0;
  if (motorDir != lastDir || motorSpeed != lastSpeed) {
    lastDir = motorDir;
    lastSpeed = motorSpeed;
    Serial.print("Motor Direction: ");
    Serial.println(motorDir);
    Serial.print("Motor Speed: ");
    Serial.println(motorSpeed);
  }
}

void motorForward(int speed) {
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  analogWrite(5, speed);
}

void motorBackward(int speed) {
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  analogWrite(5, speed);
}
