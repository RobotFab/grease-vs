// snippet: sensor_temp_40
// tags: temperature, sensor, millis, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Read DHT11 on pin 7
// prompts: dht11, compute, index, temperature, humidity, declare
// verified: compile-only
// source: kd-generated (id 40)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
float t = 0.0;
float h = 0.0;
float heatIndex = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 2000) {
    prevMs = now;
    t = dht.readTemperature();
    h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("DHT read failed");
      return;
    }

    // Compute heat index using Steadman formula
    heatIndex = -8.784695 + 1.61139411 * t + 2.338549 * h - 0.14611605 * t * h
                - 0.012308094 * t * t - 0.016424828 * h * h + 0.002211732 * t * t * h
                + 0.00072546 * t * h * h - 0.000003582 * t * t * h * h;

    Serial.print("temp: "); Serial.print(t);
    Serial.print(" hum: "); Serial.print(h);
    Serial.print(" hi: "); Serial.println(heatIndex);
  }
}
