// snippet: ds18b20_onewire
// tags: temperature, ds18b20, onewire, 1wire, dallas, waterproof, probe
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: Read temperature from DS18B20 (1-Wire) using DallasTemperature library
// prompts: DS18B20, OneWire temperature, Dallas temperature, 1-Wire sensor, waterproof temperature probe, multiple temperature sensors
// exact-include: OneWire.h, DallasTemperature.h

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 4
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
