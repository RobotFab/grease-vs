// snippet: decl_practice_136
// tags: globals, declare, millis, nonblocking, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Declare unsigned long prevMillis=0 at global scope
// prompts: declare, unsigned, prevmillis=0, scope, if(millis()-prevmillis, interval)
// verified: compile-only
// source: kd-generated (id 136)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
unsigned long prevMillis = 0;
const unsigned long INTERVAL = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMillis >= INTERVAL) {
    prevMillis = now;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }
}
