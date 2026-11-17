// snippet: hc05_bluetooth
// tags: bluetooth, hc05, hc06, softwareserial, serial, wireless, uart
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Send and receive data over Bluetooth using an HC-05 or HC-06 module.
// prompts: HC-05 Bluetooth, HC-06 Bluetooth, Bluetooth serial, wireless UART, send data Bluetooth, Bluetooth module Arduino
//   HC-05/HC-06 appear as a serial port on the phone/PC side (e.g. via Serial Bluetooth Terminal).
//   Wiring: module TX → RX_PIN, module RX → TX_PIN (use a voltage divider on TX_PIN if 5V board),
//   VCC → 5V, GND → GND. Default baud rate from factory is 9600.
//
// ---- CONFIGURE ----
// exact-include: SoftwareSerial.h
#define BT_RX_PIN  10   // Arduino pin connected to HC-05 TX
#define BT_TX_PIN  11   // Arduino pin connected to HC-05 RX
#define BT_BAUD    9600
// -------------------

#include <SoftwareSerial.h>

SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);

void setup() {
  Serial.begin(115200);
  bt.begin(BT_BAUD);
  Serial.println("HC-05 ready. Send text from phone.");
}

void loop() {
  // Forward phone → Serial monitor
  if (bt.available()) {
    char c = bt.read();
    Serial.write(c);
  }
  // Forward Serial monitor → phone
  if (Serial.available()) {
    char c = Serial.read();
    bt.write(c);
  }
}
