// snippet: decl_practice_133
// tags: globals, declare, millis, nonblocking, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Declare all global variables before setup(): float temperature=0
// prompts: declare, variables, before, setup(), float, temperature=0
// verified: compile-only
// source: kd-generated (id 133)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0;
float humidity = 0.0;
float heatIndex = 0.0;
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

    heatIndex = -8.784695 + 1.61139411 * temperature + 2.338549 * humidity - 0.14611605 * temperature * humidity
                - 0.012308094 * temperature * temperature - 0.016424828 * humidity * humidity + 0.002211732 * temperature * temperature * humidity
                + 0.00072546 * temperature * humidity * humidity - 0.000003582 * temperature * temperature * humidity * humidity;

    Serial.print("temp: "); Serial.print(temperature);
    Serial.print(" hum: "); Serial.print(humidity);
    Serial.print(" hi: "); Serial.println(heatIndex);
  }
}
