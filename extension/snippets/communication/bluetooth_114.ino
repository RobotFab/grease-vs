// snippet: bluetooth_114
// tags: bluetooth, hc05, serial, wireless, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: HC-05 Bluetooth bridge: mirror hardware Serial to SoftwareSerial(2,3) bidirectionally
// prompts: hc-05, bluetooth, bridge, mirror, hardware, softwareserial(2
// verified: compile-only
// source: kd-generated (id 114)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial btSerial(2, 3);  // RX=2, TX=3

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  Serial.println("HC-05 ready. Send text from phone.");
}

void loop() {
  // Forward phone → Serial monitor
  if (btSerial.available()) {
    char c = btSerial.read();
    Serial.write(c);
  }
  // Forward Serial monitor → phone
  if (Serial.available()) {
    char c = Serial.read();
    btSerial.write(c);
  }
}
