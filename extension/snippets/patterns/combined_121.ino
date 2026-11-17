// snippet: combined_121
// tags: combined, multi-sensor, millis, dht, newping
// boards: avr, esp32, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: DHT, NewPing
// description: Read DHT22 on pin 4 and HC-SR04 on pins 12/13 simultaneously
// prompts: dht22, hc-sr04, 12/13, simultaneously, declare, float
// verified: compile-only
// source: kd-generated (id 121)
// exact-include: DHT.h, NewPing.h

#include <DHT.h>
#include <NewPing.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define TRIGGER 12
#define ECHO 13
#define MAX_CM 400

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER, ECHO, MAX_CM);

float temperature = 0.0;
float humidity = 0.0;
float distanceCm = 0.0;

unsigned long prevMs = 0;
const unsigned long INTERVAL = 500;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    // Read DHT22
    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (!isnan(newTemp) && !isnan(newHum)) {
      temperature = newTemp;
      humidity = newHum;
    }

    // Read HC-SR04
    int newDist = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    if (newDist != 0) {
      distanceCm = newDist;
    }

    // Print CSV
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(humidity);
    Serial.print(",");
    Serial.println(distanceCm);
  }
}
