// snippet: decl_practice_141
// tags: globals, declare, millis, nonblocking, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Declare SoftwareSerial btSerial(2,3) at global scope
// prompts: declare, softwareserial, btserial(2, scope, btserial, begin(9600)
// verified: compile-only
// source: kd-generated (id 141)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial btSerial(2, 3);  // RX, TX

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  Serial.println("Bluetooth ready");
}

void loop() {
  if (btSerial.available()) {
    char c = btSerial.read();
    Serial.write(c);
  }
}
