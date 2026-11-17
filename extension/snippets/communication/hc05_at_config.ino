// snippet: hc05_at_config
// tags: bluetooth, hc05, at, config, baud, name, pair, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Enter AT command mode on HC-05 to change name, baud rate, or PIN.
// prompts: HC-05 AT commands, configure Bluetooth module, change HC-05 name, HC-05 baud rate, AT command mode
//   Hold the HC-05 KEY/EN pin HIGH before powering on to enter AT mode (LED blinks slowly).
//   In AT mode the module communicates at 38400 baud regardless of configured data baud.
//   Type commands into Serial Monitor (line ending: Both NL & CR).
//   Common commands: AT  AT+NAME=MyBot  AT+UART=115200,0,0  AT+PSWD=1234
//
// ---- CONFIGURE ----
// exact-include: SoftwareSerial.h
#define BT_RX_PIN  10
#define BT_TX_PIN  11
#define AT_BAUD    38400
// -------------------

#include <SoftwareSerial.h>

SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);

void setup() {
  Serial.begin(115200);
  bt.begin(AT_BAUD);
  Serial.println("HC-05 AT mode bridge. Type AT commands below.");
  Serial.println("If no response, check KEY pin is HIGH and baud is 38400.");
}

void loop() {
  if (bt.available())     Serial.write(bt.read());
  if (Serial.available()) bt.write(Serial.read());
}
