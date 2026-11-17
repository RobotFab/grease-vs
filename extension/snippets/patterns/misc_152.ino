// snippet: misc_152
// tags: misc, millis, nonblocking, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Sensor warmup: DHT22 on pin 4
// prompts: sensor, warmup, dht22, declare, warmedup=false, unsigned
// verified: compile-only
// source: kd-generated (id 152)
// exact-include: DHT.h

#include <DHT.h>

// Global variables for sensor warmup
bool warmedUp = false;
unsigned long startMs = 0;
const unsigned long WARMUP_TIME = 2000; // 2 seconds

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  startMs = millis();
}

void loop() {
  unsigned long now = millis();

  // Wait for warmup
  if (!warmedUp && now - startMs >= WARMUP_TIME) {
    warmedUp = true;
  }

  // Read sensor after warmup
  if (warmedUp) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT read failed");
      return;
    }

    Serial.print("temp: "); Serial.print(temp);
    Serial.print(" hum: "); Serial.println(hum);
  }
}
