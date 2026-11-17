// snippet: software_serial
// tags: uart, serial, softwareserial, rx, tx, communication, second, pins
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: Create a second serial port on arbitrary pins using SoftwareSerial
// prompts: software serial, second UART, SoftwareSerial port, extra serial pins, serial on any pin, software UART
// exact-include: SoftwareSerial.h
// NOTE: SoftwareSerial.h is AVR-core-only. RP2040 should use Serial1/SerialPIO,
//   ESP32 has 3 hardware UARTs (Serial1/Serial2), nano33iot has Serial1.

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  // RX=10, TX=11

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("SoftwareSerial ready");
}

void loop() {
  if (mySerial.available()) {
    char c = mySerial.read();
    Serial.write(c);
  }
  if (Serial.available()) {
    char c = Serial.read();
    mySerial.write(c);
  }
}
