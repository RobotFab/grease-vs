// snippet: esp32_sensor_159
// tags: esp32, sensor, millis, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: ESP32: read DHT22 on pin 4 every 2 seconds using millis
// prompts: esp32, dht22, seconds, declare, dht(4, dht22)
// verified: compile-only
// source: kd-generated (id 159)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
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
