// snippet: bh1750_light_sensor
// tags: input, bh1750, light-sensor, lux, i2c, ambient-light
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: BH1750
// description: Read ambient light level (lux) from BH1750 I2C sensor
// prompts: BH1750, light level, lux, ambient light, BH1750 I2C, light meter
// exact-include: Wire.h, BH1750.h

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  float lux = lightMeter.readLightLevel();

  Serial.print("Light level: ");
  Serial.print(lux);
  Serial.println(" lux");
}
