// snippet: bluetooth_117
// tags: bluetooth, hc05, serial, wireless, softwareserial, dht
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial, DHT
// description: BLE HM-10 sensor broadcaster: Declare SoftwareSerial bleSerial(2,3) globally
// prompts: hm-10, sensor, broadcaster, declare, softwareserial, bleserial(2
// verified: compile-only
// source: kd-generated (id 117)
// exact-include: SoftwareSerial.h, DHT.h

#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial bleSerial(2, 3);  // RX=2, TX=3
DHT dht(4, DHT22);

unsigned long prevMs = 0;
const unsigned long INTERVAL = 500;

void setup() {
  Serial.begin(9600);
  bleSerial.begin(9600);
  dht.begin();
  Serial.println("DHT22 and BLE setup complete");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT read failed");
      return;
    }

    bleSerial.print("T:");
    bleSerial.print(temp);
    bleSerial.print(",H:");
    bleSerial.println(hum);
  }
}
