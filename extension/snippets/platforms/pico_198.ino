// snippet: pico_198
// tags: pico, rp2040, raspberry-pi, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: RPi Pico: read DHT22 on pin 6
// prompts: DHT22 Pico, temperature humidity RP2040, DHT sensor Raspberry Pi Pico, read temperature Pi Pico, humidity sensor RP2040, DHT22 Pi Pico
// verified: compile-only
// source: kd-generated (id 198)
// exact-include: DHT.h

#include <DHT.h>

DHT dht(6, DHT22);
float temperature, humidity;
unsigned long prev = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prev >= 2000) {
    prev = now;
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("DHT read failed");
      return;
    }

    Serial.print("temp: "); Serial.print(temperature);
    Serial.print(" hum: "); Serial.println(humidity);
  }
}
