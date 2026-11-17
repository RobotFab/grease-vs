// snippet: serial_report_93
// tags: serial, report, uart, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Heartbeat: regardless of other activity, print ALIVE:<millis()> every 5 seconds using millis
// prompts: heartbeat, regardless, other, activity, alive, <millis()>
// verified: compile-only
// source: kd-generated (id 93)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

const int LED_PIN = LED_BUILTIN;
const unsigned long HEARTBEAT_INTERVAL = 5000;
const unsigned long COMMAND_INTERVAL = 100;

unsigned long prevHeartbeatMs = 0;
unsigned long prevCommandMs = 0;
String inputBuf = "";
bool lineReady = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void processCommand(const String& cmd) {
  if (cmd == "PING") {
    Serial.println("PONG");
  } else if (cmd == "STATUS") {
    Serial.print("ALIVE: "); Serial.println(millis());
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}

void loop() {
  // Handle heartbeat
  unsigned long now = millis();
  if (now - prevHeartbeatMs >= HEARTBEAT_INTERVAL) {
    prevHeartbeatMs = now;
    Serial.print("ALIVE: "); Serial.println(now);
  }

  // Handle serial commands
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

  // Non-blocking work can go here
}
