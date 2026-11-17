// snippet: combined_129
// tags: combined, multi-sensor, millis, adafruit_sensor, adafruit_bme280
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_Sensor, Adafruit_BME280
// description: Environmental station with serial report: BME280 over I2C
// prompts: environmental, station, serial, report, bme280, declare
// verified: compile-only
// source: kd-generated (id 129)
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
  // Handle serial STATUS requests inline (serialEvent() does not fire on ESP32)
  while (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "STATUS") {
      Serial.print("temp: "); Serial.print(temp);
      Serial.print(" hum: "); Serial.print(hum);
      Serial.print(" pres: "); Serial.print(pres);
      Serial.print(" alt: "); Serial.println(alt);
    }
  }

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
