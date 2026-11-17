// snippet: bme280_basic
// tags: environmental, bme280, temperature, humidity, pressure, altitude, adafruit, i2c, spi
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit BME280, Adafruit Unified Sensor
// description: Read temperature, humidity, and pressure from Adafruit BME280
// prompts: BME280, barometric pressure, temperature humidity pressure, altitude sensor, weather sensor, Adafruit BME280, atmospheric sensor
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_BME280.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!bme.begin(0x76)) {
    if (!bme.begin(0x77)) {
      Serial.println("BME280 not found");
      while (1);
    }
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  Serial.print("temp: ");    Serial.print(bme.readTemperature());
  Serial.print(" hum: ");    Serial.print(bme.readHumidity());
  Serial.print(" pres: ");   Serial.println(bme.readPressure() / 100.0F);
}
