// snippet: bluetooth_120
// tags: bluetooth, hc05, serial, wireless, softwareserial, dht
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial, DHT
// description: Dual serial: hardware Serial for debug, SoftwareSerial(2,3) for BLE HM-10
// prompts: serial, hardware, debug, softwareserial(2, hm-10, declare
// verified: compile-only
// source: kd-generated (id 120)
// exact-include: SoftwareSerial.h, DHT.h

#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial bleSerial(2, 3);  // RX, TX
DHT dht(4, DHT22);

void setup() {
  Serial.begin(9600);
  bleSerial.begin(9600);
  dht.begin();
  Serial.println("Dual serial setup complete");
}

void loop() {
  if (bleSerial.available()) {
    String command = bleSerial.readStringUntil('\n');
    if (command == "TEMP") {
      float temperature = dht.readTemperature();
      if (isnan(temperature)) {  // a failed DHT read returns NaN — report, don't send garbage
        bleSerial.println("TEMP: error");
      } else {
        bleSerial.print("TEMP: ");
        bleSerial.println(temperature);
      }
    }
  }
}
