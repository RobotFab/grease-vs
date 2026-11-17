// snippet: serial_report_94
// tags: serial, report, uart, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Streaming sensor: after START <hz> command, continuously print A0 reading at that rate
// prompts: streaming, sensor, after, start, command, continuously
// verified: compile-only
// source: kd-generated (id 94)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

const int A0_PIN = A0;
const unsigned long COMMAND_INTERVAL = 100;

bool streaming = false;
unsigned long streamPrev = 0;
long streamInterval = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: START <hz>  STOP");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) return;

    if (cmd.startsWith("START")) {
      int hz = cmd.substring(6).toInt();
      if (hz > 0) {
        streaming = true;
        streamInterval = 1000 / hz;
        streamPrev = millis();
      } else {
        Serial.println("ERR invalid hz");
      }
    } else if (cmd == "STOP") {
      streaming = false;
    }
  }

  if (streaming) {
    unsigned long now = millis();
    if (now - streamPrev >= streamInterval) {
      streamPrev = now;
      int raw = analogRead(A0_PIN);
      Serial.print("A0:"); Serial.println(raw);
    }
  }
}
