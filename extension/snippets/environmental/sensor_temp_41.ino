// snippet: sensor_temp_41
// tags: temperature, sensor, millis, onewire, dallastemperature
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: Read DS18B20 on OneWire pin 3
// prompts: ds18b20, onewire, include, onewire, dallastemperature, declare
// verified: compile-only
// source: kd-generated (id 41)
// exact-include: OneWire.h, DallasTemperature.h

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 3
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  sensors.requestTemperatures();  // Dallas: trigger a conversion on the 1-Wire bus (blocks ~750ms)
  float tempC = sensors.getTempCByIndex(0);  // Dallas: Celsius for device index (0-based)
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("DS18B20 not found");
    return;
  }
  Serial.print("temp: ");
  Serial.println(tempC);
}
