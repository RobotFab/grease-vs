// snippet: motor_dc_1
// tags: motor, dc, pwm, speed, direction, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: Drive a DC motor via L298N on pins ENA=5, IN1=6, IN2=7
// prompts: drive, motor, l298n, ena=5, in1=6, in2=7
// verified: compile-only
// source: kd-generated (id 1)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

const int ENA = 5, IN1 = 6, IN2 = 7;
const int SERIAL_BAUD = 9600;

SoftwareSerial Serial2(10, 11); // RX, TX

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial2.begin(SERIAL_BAUD);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
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

void loop() {
  static String input = "";
  while (Serial2.available()) {
    char c = Serial2.read();
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
}
