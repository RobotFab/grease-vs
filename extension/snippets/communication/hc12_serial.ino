// snippet: hc12_serial
// tags: wireless, hc12, 433mhz, serial, uart, long-range, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial
// description: Send data over HC-12 433 MHz serial wireless module — transparent UART bridge
// prompts: HC-12 wireless, 433MHz radio, long range serial, wireless UART link, HC12 module, transparent serial bridge
// exact-include: SoftwareSerial.h

#include <SoftwareSerial.h>

SoftwareSerial hc12(10, 11);  // RX=10, TX=11
unsigned long prevMs = 0;
int count = 0;

void setup() {
  Serial.begin(9600);
  hc12.begin(9600);
  Serial.println("HC-12 ready");
}

void loop() {
  // relay HC-12 → USB Serial
  while (hc12.available()) Serial.write(hc12.read());

  // send every 2 s
  unsigned long now = millis();
  if (now - prevMs >= 2000) {
    prevMs = now;
    hc12.print("PKT:"); hc12.println(count++);
  }
}
