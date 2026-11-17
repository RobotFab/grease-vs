// snippet: bluetooth_115
// tags: bluetooth, hc05, serial, wireless, softwareserial, servo
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial, Servo
// description: HM-10 BLE on SoftwareSerial(2,3)
// prompts: hm-10, softwareserial(2, declare, softwareserial, bleserial(2, globally
// verified: compile-only
// source: kd-generated (id 115)
// exact-include: SoftwareSerial.h, Servo.h

#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial bleSerial(2, 3);  // RX, TX
Servo myServo;

void setup() {
  bleSerial.begin(9600);
  myServo.attach(9);
  Serial.begin(9600);
  Serial.println("BLE Servo Control Ready");
}

void loop() {
  if (bleSerial.available()) {
    String cmd = bleSerial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("SERVO")) {
      int angle = cmd.substring(5).toInt();
      myServo.write(angle);
      bleSerial.println("DONE");
    }
  }
}
