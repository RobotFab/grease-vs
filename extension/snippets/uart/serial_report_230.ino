// snippet: serial_report_230
// tags: serial, report, uart, dht, newping
// boards: avr, esp32, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: DHT, NewPing
// description: Serial plotter multi-channel: simultaneously print DHT22 temperature (pin 4) and HC-SR04 distance…
// prompts: plotter, multi-channel, simultaneously, dht22, temperature, hc-sr04
// verified: compile-only
// source: kd-generated (id 230)
// exact-include: DHT.h, NewPing.h

#include <DHT.h>
#include <NewPing.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define TRIG_PIN 12
#define ECHO_PIN 13
#define MAX_DISTANCE 200

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned long prevDHTMs = 0;
unsigned long prevSonarMs = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  unsigned long now = millis();

  // DHT22 temperature reading
  if (now - prevDHTMs >= 500) {
    prevDHTMs = now;
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {  // a failed DHT read returns NaN — skip, don't feed the plotter garbage
      Serial.print("temp:");
      Serial.print(temperature, 2);
      Serial.print(",");
    }
  }

  // HC-SR04 distance reading
  if (now - prevSonarMs >= 500) {
    prevSonarMs = now;
    int distance = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    Serial.print("dist:");
    Serial.print(distance);
    Serial.println();
  }
}
