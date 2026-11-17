// snippet: pico_204
// tags: pico, rp2040, raspberry-pi, softwareserial
// boards: rp2040
// not-tags: romi32u4, avr, megaavr
// library: SoftwareSerial
// description: RPi Pico: SoftwareSerial on pins 12/13 for HC-05
// prompts: Bluetooth Pico, HC-05 RP2040, SoftwareSerial Raspberry Pi Pico, HC-05 Pi Pico, Bluetooth serial RP2040, wireless serial Pico
// verified: compile-only
// source: kd-generated (id 204)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial btSerial(12, 13);

void setup() {
  Serial.begin(115200);
  btSerial.begin(9600);
  Serial.println("HC-05 ready. Send PING.");
}

void loop() {
  if (btSerial.available()) {
    String cmd = btSerial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "PING") {
      btSerial.println("PONG");
    }
  }
}
