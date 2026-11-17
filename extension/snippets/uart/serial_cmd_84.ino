// snippet: serial_cmd_84
// tags: serial, command, parser, uart, softwareserial, stream
// boards: avr, megaavr
// library: SoftwareSerial
// description: STREAM ON / STREAM OFF commands arriving on a SoftwareSerial port, with a 10 Hz
//   analog stream in between. Accumulate the line one character at a time into a fixed char
//   buffer and act on '\n'.
//   NOT: readStringUntil('\n'). It BLOCKS until the terminator arrives or the 1000 ms stream
//   timeout expires — a partial line stalls the whole sketch for a full second, which on a
//   moving robot is a second of nobody watching the sensors. It also allocates a String on a
//   2 KB AVR, which fragments the heap over hours of running.
//   NOT on an ATmega32U4 (Leonardo / Micro / Pololu Romi / A-Star): SoftwareSerial RX only
//   works on pin-change-interrupt pins there, and pin 10 is the Romi's LEFT MOTOR PWM output.
//   Use the hardware UART Serial1 (pins 0/1) on those boards. Same on ESP32 (Serial1/Serial2)
//   and RP2040 (Serial1 or SerialPIO).
// prompts: STREAM ON STREAM OFF, serial command processor, SoftwareSerial commands, start stop streaming serial, parse serial command non-blocking
// verified: compile-only
// source: kd-generated (id 84)
// not-tags: romi32u4, romi, leonardo, esp32, nano33iot, rp2040
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  // RX=10, TX=11 — Uno/Nano/Mega only (see header)

char cmdBuf[24];
uint8_t cmdIdx = 0;

bool streaming = false;
unsigned long streamPrev = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Serial command processor ready");
}

void handleCommand(const char* cmd) {
  if (strcmp(cmd, "STREAM ON") == 0) {
    streaming = true;
    Serial.println("Streaming ON");
  } else if (strcmp(cmd, "STREAM OFF") == 0) {
    streaming = false;
    Serial.println("Streaming OFF");
  }
}

void loop() {
  // Drain whatever has arrived and return — never wait for the rest of a line.
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      cmdBuf[cmdIdx] = '\0';
      handleCommand(cmdBuf);
      cmdIdx = 0;
    } else if (c != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = c;
    }
  }

  if (streaming && millis() - streamPrev >= 100) {  // 10 Hz
    streamPrev = millis();
    Serial.print("A0: ");
    Serial.println(analogRead(A0));
  }
}
