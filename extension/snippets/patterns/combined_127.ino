// snippet: combined_127
// tags: combined, multi-sensor, millis, onewire, dallastemperature
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: Temperature-controlled fan: DS18B20 pin 3
// prompts: temperature-controlled, ds18b20, declare, float, tempc, globally
// verified: compile-only
// source: kd-generated (id 127)
// exact-include: OneWire.h, DallasTemperature.h

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 3
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
float tempC = 0.0;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(5, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  sensors.requestTemperatures();  // Dallas: trigger a conversion on the 1-Wire bus (blocks ~750ms)
  tempC = sensors.getTempCByIndex(0);  // Dallas: Celsius for device index (0-based)
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("DS18B20 not found");
    return;
  }

  int fanSpeed = map(tempC, 25.0, 40.0, 0, 255);
  analogWrite(5, fanSpeed);

  Serial.print("temp: ");
  Serial.print(tempC);
  Serial.print("°C  fan speed: ");
  Serial.println(fanSpeed);
}
