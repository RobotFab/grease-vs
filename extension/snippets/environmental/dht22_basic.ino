// snippet: dht22_basic
// tags: environmental, dht22, dht11, dht, temperature, humidity, sensor
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT sensor library
// description: Read temperature and humidity from DHT22 (or DHT11) sensor.
//   Heat index: Arduino has NO heatindex() built-in. Compute manually with Steadman formula:
//     float hi = -8.784695 + 1.61139411*t + 2.338549*h - 0.14611605*t*h
//                - 0.012308094*t*t - 0.016424828*h*h + 0.002211732*t*t*h
//                + 0.00072546*t*h*h - 0.000003582*t*t*h*h;
//   (valid for t >= 27°C and h >= 40%). For simpler approximation: hi = t + 0.33*h - 4.0
// prompts: DHT22 sensor, DHT11 sensor, read temperature humidity, temperature and humidity, humidity sensor, dht temperature, dht22 pin, heat index DHT22, compute heat index temperature humidity
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN  2
#define DHTTYPE DHT22   // or DHT11

DHT dht(DHTPIN, DHTTYPE);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed");
    return;
  }

  Serial.print("temp: "); Serial.print(temp);
  Serial.print(" hum: "); Serial.println(hum);
}
