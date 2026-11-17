// snippet: serial_cmd_83
// tags: serial, command, parser, uart, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Serial timeout watchdog: if no command received within 5 seconds, stop motor (pin 5 to 0) and pri…
// prompts: timeout, watchdog, command, received, within, seconds
// verified: compile-only
// source: kd-generated (id 83)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  // RX=10, TX=11
const unsigned long TIMEOUT_MS = 5000;
unsigned long lastCmdMs = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);  // Ensure motor is off initially
  Serial.println("Serial timeout watchdog ready");
}

void loop() {
  if (mySerial.available()) {
    // Drain the RX buffer — if bytes are never read, available() stays true
    // and the watchdog can never re-time-out after the first byte arrives.
    while (mySerial.available()) {
      char c = mySerial.read();
      // Process command bytes here (e.g. accumulate into a line buffer)
    }
    lastCmdMs = millis();
  }

  unsigned long now = millis();
  if (now - lastCmdMs >= TIMEOUT_MS) {
    digitalWrite(5, LOW);  // Stop motor
    Serial.println("TIMEOUT");
  }
}
