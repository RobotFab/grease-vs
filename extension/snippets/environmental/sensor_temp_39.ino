// snippet: sensor_temp_39
// tags: temperature, sensor, millis, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Read DHT22 on pin 4
// prompts: dht22, declare, float, temperature, humidity, scope
// verified: compile-only
// source: kd-generated (id 39)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0;
float humidity = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 2000) {
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
