// snippet: sensor_temp_45
// tags: temperature, sensor, millis, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: DHT22 temperature alarm on pin 4
// prompts: dht22, temperature, alarm, declare, float, temperature
// verified: compile-only
// source: kd-generated (id 45)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0;
unsigned long prevMs = 0;
const unsigned long CHECK_INTERVAL = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= CHECK_INTERVAL) {
    prevMs = now;
    temperature = dht.readTemperature();

    if (isnan(temperature)) {
      Serial.println("DHT read failed");
      return;
    }

    Serial.print("temp: "); Serial.println(temperature);

    if (temperature > 30.0) {
      Serial.println("HIGH TEMP");
    }
  }
}
