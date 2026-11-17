// snippet: interrupt_driven_433
// tags: softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: Implement an emergency stop: attach an interrupt to pin 2 (FALLING)
// prompts: implement, emergency, attach, interrupt, (falling), triggered
// verified: compile-only
// source: kd-generated (id 433)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

const int ENA = 5, IN1 = 6, IN2 = 7;
const int SERIAL_BAUD = 9600;

SoftwareSerial Serial2(10, 11); // RX, TX

volatile bool emergencyStop = false;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial2.begin(SERIAL_BAUD);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), emergencyStopHandler, FALLING);
  Serial.println("Ready. Commands: FWD <speed>, REV <speed>, STOP");
}

void driveMotor(int speed, int direction) {
  if (direction == 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (direction == -1) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  analogWrite(ENA, abs(speed));
}

void emergencyStopHandler() {
  emergencyStop = true;
}

void loop() {
  static String input = "";
  if (Serial2.available()) {
    // Fix: Cast byte to char before appending to String
    // Serial2.read() returns int (byte value), String += int converts to decimal text
    // Using (char) ensures we append the actual character
    char c = (char)Serial2.read();
    if (c == '\n') {
      input.trim();
      if (input.startsWith("FWD")) {
        int speed = input.substring(3).toInt();
        driveMotor(speed, 1);
      } else if (input.startsWith("REV")) {
        int speed = input.substring(3).toInt();
        driveMotor(speed, -1);
      } else if (input == "STOP") {
        driveMotor(0, 0);
      }
      input = "";
    } else if (c != '\r') {
      input += c;
    }
  }

  if (emergencyStop) {
    driveMotor(0, 0);
  }
}
