// snippet: bluetooth_116
// tags: bluetooth, hc05, serial, wireless, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: HC-05 on SoftwareSerial(2,3): parse CSV command '5,1' meaning set pin 5 HIGH
// prompts: hc-05, softwareserial(2, parse, command, meaning, declare
// verified: compile-only
// source: kd-generated (id 116)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial btSerial(2, 3);  // RX, TX

void setup() {
  btSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("HC-05 Command Parser Ready");
}

void loop() {
  if (btSerial.available()) {
    String cmd = btSerial.readStringUntil('\n');
    cmd.trim();
    if (cmd.indexOf(',') != -1) {
      int pin = cmd.substring(0, cmd.indexOf(',')).toInt();
      int value = cmd.substring(cmd.indexOf(',') + 1).toInt();
      if (pin >= 2 && pin <= 13) {
        digitalWrite(pin, value);
        btSerial.print("SET ");
        btSerial.print(pin);
        btSerial.print("=");
        btSerial.println(value);
      } else {
        btSerial.println("ERR Invalid pin");
      }
    }
  }
}
