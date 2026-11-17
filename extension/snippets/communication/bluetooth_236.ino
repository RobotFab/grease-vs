// snippet: bluetooth_236
// tags: bluetooth, hc05, serial, wireless, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: BLE HM-10 transparent mode: all bytes from SoftwareSerial(2,3) forwarded to Serial and vice versa
// prompts: hm-10, transparent, bytes, softwareserial(2, forwarded, versa
// verified: compile-only
// source: kd-generated (id 236)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial bleSerial(2, 3);  // RX, TX

void setup() {
  Serial.begin(9600);
  bleSerial.begin(9600);
  Serial.println("BLE HM-10 transparent mode. Type text below.");
}

void loop() {
  // Forward BLE → Serial
  if (bleSerial.available()) {
    Serial.write(bleSerial.read());
  }

  // Forward Serial → BLE
  if (Serial.available()) {
    bleSerial.write(Serial.read());
  }
}
