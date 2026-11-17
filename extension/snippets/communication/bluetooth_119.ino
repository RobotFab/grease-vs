// snippet: bluetooth_119
// tags: bluetooth, hc05, serial, wireless, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: BLE gamepad via HM-10: receive X:<val> Y:<val> over SoftwareSerial(2,3)
// prompts: gamepad, hm-10, receive, <val>, <val>, softwareserial(2
// verified: compile-only
// source: kd-generated (id 119)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial ble(2, 3);  // RX, TX
int leftSpeed = 0;
int rightSpeed = 0;

void setup() {
  pinMode(12, OUTPUT);  // IN1
  pinMode(4, OUTPUT);   // IN2
  pinMode(5, OUTPUT);   // ENA
  pinMode(6, OUTPUT);   // IN3
  pinMode(7, OUTPUT);   // IN4
  pinMode(8, OUTPUT);   // ENB
  Serial.begin(9600);
  ble.begin(9600);
  Serial.println("BLE gamepad ready");
}

void loop() {
  if (ble.available()) {
    String line = ble.readStringUntil('\n');
    if (line.startsWith("X:")) {
      leftSpeed = map(line.substring(2).toInt(), 0, 100, 0, 255);
    } else if (line.startsWith("Y:")) {
      rightSpeed = map(line.substring(2).toInt(), 0, 100, 0, 255);
      motorLeft(leftSpeed);
      motorRight(rightSpeed);
    }
  }
}

void motorLeft(int speed) {
  digitalWrite(12, speed > 127);  // IN1
  digitalWrite(4, speed <= 127); // IN2
  analogWrite(5, abs(speed));    // ENA
}

void motorRight(int speed) {
  digitalWrite(6, speed > 127);  // IN3
  digitalWrite(7, speed <= 127); // IN4
  analogWrite(8, abs(speed));    // ENB
}
