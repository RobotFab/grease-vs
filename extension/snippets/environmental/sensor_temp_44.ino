// snippet: sensor_temp_44
// tags: temperature, sensor, millis, adafruit_sensor, adafruit_bme280
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_Sensor, Adafruit_BME280
// description: Read BME280 over I2C
// prompts: bme280, declare, adafruit_bme280, globally, print, temperature
// verified: compile-only
// source: kd-generated (id 44)
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_BME280.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;
float temp, hum, pres, alt;
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

  temp = bme.readTemperature();
  hum = bme.readHumidity();
  pres = bme.readPressure() / 100.0F;
  alt = bme.readAltitude(1013.25);

  Serial.print("temp: "); Serial.print(temp);
  Serial.print(" hum: "); Serial.print(hum);
  Serial.print(" pres: "); Serial.print(pres);
  Serial.print(" alt: "); Serial.println(alt);
}
