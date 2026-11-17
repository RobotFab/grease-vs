// snippet: bluetooth_235
// tags: bluetooth, hc05, serial, wireless, softwareserial
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial
// description: HC-05 AT command mode: if no BT data for 10 seconds, pull AT pin HIGH, enter AT mode, print AT:READY
// prompts: hc-05, command, seconds, enter, ready, arrives
// verified: compile-only
// source: kd-generated (id 235)
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

#define BT_RX_PIN  10
#define BT_TX_PIN  11
#define AT_BAUD    38400
#define AT_PIN     2

SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);
unsigned long lastBtMs = 0;
bool atLatched = false;

void setup() {
  Serial.begin(115200);
  bt.begin(AT_BAUD);
  pinMode(AT_PIN, OUTPUT);
  digitalWrite(AT_PIN, LOW);
  Serial.println("HC-05 AT mode bridge. Type AT commands below.");
  Serial.println("If no response, check KEY pin is HIGH and baud is 38400.");
}

void loop() {
  if (bt.available()) {
    lastBtMs = millis();
    digitalWrite(AT_PIN, LOW);
    atLatched = false;
    Serial.write(bt.read());
  }

  if (millis() - lastBtMs > 10000 && !atLatched) {
    digitalWrite(AT_PIN, HIGH);
    Serial.println("AT:READY");
    atLatched = true;
  }
}
