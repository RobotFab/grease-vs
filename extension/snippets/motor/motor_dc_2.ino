// snippet: motor_dc_2
// tags: motor, dc, pwm, speed, direction, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: Dual DC motor via L298N: motor A on pins 5/6/7, motor B on pins 9/10/11
// prompts: motor, l298n, motor, 5/6/7, motor, 9/10/11
// verified: compile-only
// source: kd-generated (id 2)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

const int ENA = 5, IN1 = 6, IN2 = 7;
const int ENB = 9, IN3 = 10, IN4 = 11;
const int SERIAL_BAUD = 9600;

SoftwareSerial Serial2(8, 12); // RX=8, TX=12 (avoid 10/11 used by motor B)

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial2.begin(SERIAL_BAUD);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.println("Ready. Commands: LEFT <speed>, RIGHT <speed>, FWD <speed>, REV <speed>, STOP");
}

void driveMotor(int motor, int speed, int direction) {
  if (motor == 1) {
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
  } else if (motor == 2) {
    if (direction == 1) {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    } else if (direction == -1) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    } else {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
    analogWrite(ENB, abs(speed));
  }
}

void loop() {
  static String input = "";
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      input.trim();
      if (input.startsWith("LEFT")) {
        int speed = input.substring(4).toInt();
        driveMotor(1, speed, 1);
      } else if (input.startsWith("RIGHT")) {
        int speed = input.substring(5).toInt();
        driveMotor(2, speed, 1);
      } else if (input.startsWith("FWD")) {
        int speed = input.substring(3).toInt();
        driveMotor(1, speed, 1);
        driveMotor(2, speed, 1);
      } else if (input.startsWith("REV")) {
        int speed = input.substring(3).toInt();
        driveMotor(1, speed, -1);
        driveMotor(2, speed, -1);
      } else if (input == "STOP") {
        driveMotor(1, 0, 0);
        driveMotor(2, 0, 0);
      }
      input = "";
    } else if (c != '\r') {
      input += c;
    }
  }
}
