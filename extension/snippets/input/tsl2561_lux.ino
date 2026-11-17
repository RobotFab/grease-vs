// snippet: tsl2561_lux
// tags: tsl2561, lux, light, sensor, i2c, adafruit, illuminance, luminosity
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit TSL2561, Adafruit Unified Sensor
// description: Read illuminance in lux from a TSL2561 over I2C (Adafruit unified-sensor API, auto-range)
// prompts: TSL2561 lux sensor, TSL2561 light level, read lux TSL2561, luminosity sensor I2C, TSL2561 Adafruit
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_TSL2561_U.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

Adafruit_TSL2561_Unified tsl(TSL2561_ADDR_FLOAT, 12345);  // addr 0x39, sensor id
const unsigned long PRINT_MS = 1000;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!tsl.begin()) {
    Serial.println("TSL2561 not found");
    while (1);
  }
  tsl.enableAutoRange(true);  // gain switches automatically
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < PRINT_MS) return;
  prevMs = now;

  sensors_event_t event;
  tsl.getEvent(&event);  // unified Adafruit API: fills event.acceleration (m/s^2) / event.orientation (deg)
  if (event.light) {
    Serial.print("Lux: ");
    Serial.println(event.light, 1);
  } else {
    Serial.println("sensor overloaded");  // saturated in bright light
  }
}
