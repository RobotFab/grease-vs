// snippet: bluetooth_113
// tags: bluetooth, hc05, serial, wireless, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: HC-05 on SoftwareSerial rx=2 tx=3
// prompts: hc-05, softwareserial, declare, softwareserial, btserial(2, globally
// verified: compile-only
// source: kd-generated (id 113)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial btSerial(2, 3);  // RX=2, TX=3

const int EN = 9;   // PWM enable (ENA on L298N)
const int IN1 = 7;
const int IN2 = 8;

void motorForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, constrain(speed, 0, 255));
}

void motorBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, constrain(speed, 0, 255));
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 0);
}

void setup() {
  pinMode(EN, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  btSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("HC-05 ready. Send MOTOR <speed>.");
}

void loop() {
  if (btSerial.available()) {
    String command = btSerial.readStringUntil('\n');
    if (command.startsWith("MOTOR ")) {
      int speed = command.substring(6).toInt();
      if (speed > 0) {
        motorForward(speed);
      } else if (speed < 0) {
        motorBackward(-speed);
      } else {
        motorStop();
      }
      btSerial.println("OK");
    }
  }
}
