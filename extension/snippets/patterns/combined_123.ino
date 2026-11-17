// snippet: combined_123
// tags: combined, multi-sensor, millis, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Soil moisture A0 and DHT22 pin 4 environmental station
// prompts: moisture, dht22, environmental, station, declare, float
// verified: compile-only
// source: kd-generated (id 123)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float moisture = 0.0;
float temperature = 0.0;
float humidity = 0.0;

unsigned long prevMs = 0;
const unsigned long INTERVAL = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  moisture = analogRead(A0);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read failed");
    return;
  }

  Serial.print("moisture: "); Serial.println(moisture);
  Serial.print("temp: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("hum: "); Serial.print(humidity); Serial.println("%");
}
