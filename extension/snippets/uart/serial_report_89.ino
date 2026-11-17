// snippet: serial_report_89
// tags: serial, report, uart, newping
// boards: avr, esp32, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: NewPing
// description: On serial command READ, trigger HC-SR04 on pins 12/13, measure distance, reply DIST:<cm>
// prompts: serial, command, trigger, hc-sr04, 12/13, measure
// verified: compile-only
// source: kd-generated (id 89)
// exact-include: NewPing.h

#include <NewPing.h>

const int TRIGGER = 12;
const int ECHO    = 13;
const int MAX_CM  = 400;
NewPing sonar(TRIGGER, ECHO, MAX_CM);
float distCm = 0.0;
String inputBuf = "";
bool lineReady = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: READ");
}

void processCommand(const String& cmd) {
  if (cmd == "READ") {
    distCm = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    // Braces required: without them, Serial.println(distCm) executes
    // unconditionally on every call, printing a stray value after "out of range".
    if (distCm == 0) {
      Serial.println("out of range");
    } else {
      Serial.print("DIST:");
      Serial.println(distCm);
    }
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { lineReady = true; break; }
    if (c != '\r') inputBuf += c;
  }

  if (lineReady) {
    inputBuf.trim();
    if (inputBuf.length() > 0) processCommand(inputBuf);
    inputBuf = "";
    lineReady = false;
  }
}
