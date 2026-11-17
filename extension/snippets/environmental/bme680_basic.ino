// snippet: bme680_basic
// tags: environmental, bme680, bme688, temperature, humidity, pressure, gas, voc, adafruit, i2c
// boards: avr, esp32, megaavr, nano33iot, rp2040
// verified: compile-only
// library: Adafruit BME680
// description: Read temperature, humidity, pressure, and gas resistance from BME680/688
// prompts: BME680 air quality, temperature humidity pressure gas, VOC sensor, BME688 environmental sensor, air quality index
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_BME680.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

Adafruit_BME680 bme;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!bme.begin(0x76)) { Serial.println("BME680 not found"); while (1); }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);  // 320°C for 150 ms
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  if (!bme.performReading()) { Serial.println("BME680 read failed"); return; }
  Serial.print("T: "); Serial.print(bme.temperature);
  Serial.print("°C  H: "); Serial.print(bme.humidity);
  Serial.print("%  P: "); Serial.print(bme.pressure / 100.0);
  Serial.print(" hPa  Gas: "); Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" kΩ");
}
