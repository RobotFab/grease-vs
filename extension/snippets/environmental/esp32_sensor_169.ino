// snippet: esp32_sensor_169
// tags: esp32, sensor, millis, adafruit_bmp280
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_BMP280
// description: ESP32: read BMP280 over I2C
// prompts: esp32, bmp280, declare, adafruit_bmp280, globally, print
// verified: compile-only
// source: kd-generated (id 169)
// exact-include: Wire.h, Adafruit_BMP280.h

#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // Use this object for all BMP280 functions
unsigned long prevMs = 0;
const unsigned long INTERVAL = 1000; // Print every 1 second

void setup() {
  Serial.begin(9600);
  if (!bmp.begin(0x76)) { Serial.println("BMP280 not found"); while (1); }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    Serial.print("T: "); Serial.print(bmp.readTemperature()); Serial.print("°C  ");
    Serial.print("P: "); Serial.print(bmp.readPressure() / 100.0F); Serial.println(" hPa");
  }

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("TEMP")) {
      Serial.print("T: "); Serial.print(bmp.readTemperature()); Serial.println("°C");
    }
  }
}
