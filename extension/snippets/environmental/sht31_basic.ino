// snippet: sht31_basic
// tags: environmental, sht31, sht30, sht4x, temperature, humidity, adafruit, i2c, sensirion
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit SHT31
// description: Read temperature and humidity from SHT30/SHT31 with Adafruit library
// prompts: SHT31 temperature humidity, SHT30 SHT4x sensor, Sensirion humidity, high accuracy temp humidity, precise RH sensor
// exact-include: Wire.h, Adafruit_SHT31.h

#include <Wire.h>
#include <Adafruit_SHT31.h>

Adafruit_SHT31 sht31;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!sht31.begin(0x44)) { Serial.println("SHT31 not found"); while (1); }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  if (!isnan(t)) { Serial.print("T: "); Serial.print(t); Serial.print("°C  "); }
  if (!isnan(h)) { Serial.print("H: "); Serial.print(h); Serial.println("%"); }
}
