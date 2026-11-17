// snippet: scd30_co2
// tags: environmental, scd30, co2, carbon-dioxide, humidity, temperature, sparkfun, i2c
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: SparkFun SCD30 Arduino Library
// description: Read real CO2 (NDIR), temperature, and humidity from Sensirion SCD30
// prompts: SCD30 CO2 sensor, carbon dioxide monitor, NDIR CO2 measurement, Sensirion CO2 humidity, indoor air quality CO2
// exact-include: Wire.h, SparkFun_SCD30_Arduino_Library.h

#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>

SCD30 airSensor;
unsigned long prevMs = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if (!airSensor.begin()) { Serial.println("SCD30 not found"); while (1); }
  airSensor.setMeasurementInterval(2);  // seconds
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  if (airSensor.dataAvailable()) {
    Serial.print("CO2: ");    Serial.print(airSensor.getCO2()); Serial.print(" ppm  ");
    Serial.print("T: ");      Serial.print(airSensor.getTemperature(), 1); Serial.print("°C  ");
    Serial.print("RH: ");     Serial.print(airSensor.getHumidity(), 1);    Serial.println("%");
  }
}
