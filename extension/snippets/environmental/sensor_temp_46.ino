// snippet: sensor_temp_46
// tags: temperature, sensor, millis, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: DHT22 on pin 4 with moving average
// prompts: dht22, moving, average, declare, float, samples[5]
// verified: compile-only
// source: kd-generated (id 46)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float samples[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
int sampleIdx = 0;
unsigned long prevSampleMs = 0;
unsigned long prevPrintMs = 0;
const unsigned long SAMPLE_INTERVAL = 200;
const unsigned long PRINT_INTERVAL = 1000;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();

  // Sample temperature every 200ms
  if (now - prevSampleMs >= SAMPLE_INTERVAL) {
    prevSampleMs = now;
    float temp = dht.readTemperature();

    if (!isnan(temp)) {
      samples[sampleIdx] = temp;
      sampleIdx = (sampleIdx + 1) % 5;
    }
  }

  // Print 5-sample average every second
  if (now - prevPrintMs >= PRINT_INTERVAL) {
    prevPrintMs = now;
    float sum = 0.0;
    for (int i = 0; i < 5; i++) {
      sum += samples[i];
    }
    float average = sum / 5.0;
    Serial.print("average temp: "); Serial.println(average);
  }
}
