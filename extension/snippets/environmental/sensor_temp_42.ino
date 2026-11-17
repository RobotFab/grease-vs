// snippet: sensor_temp_42
// tags: temperature, sensor, millis, onewire, dallastemperature
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: Two DS18B20 sensors on one OneWire bus pin 3
// prompts: ds18b20, sensors, onewire, declare, onewire, dallastemperature
// verified: compile-only
// source: kd-generated (id 42)
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
  float temp1 = sensors.getTempCByIndex(0);  // Dallas: Celsius for device index (0-based)
  float temp2 = sensors.getTempCByIndex(1);  // Dallas: Celsius for device index (0-based)

  if (temp1 == DEVICE_DISCONNECTED_C) {
    Serial.println("DS18B20 sensor 1 not found");
    temp1 = -127.0;
  }
  if (temp2 == DEVICE_DISCONNECTED_C) {
    Serial.println("DS18B20 sensor 2 not found");
    temp2 = -127.0;
  }

  Serial.print("T1: ");
  Serial.print(temp1);
  Serial.print(" T2: ");
  Serial.println(temp2);
}
