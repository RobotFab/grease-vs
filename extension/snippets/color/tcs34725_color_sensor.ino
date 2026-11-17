// snippet: tcs34725_color_sensor
// tags: color, rgb, tcs34725, i2c, adafruit, light, r, g, b, clear
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit TCS34725
// description: Read RGBC color values and color temperature from TCS34725
// prompts: TCS34725 color sensor, RGB color detection, color temperature, detect color, light sensor RGB, color identification
// exact-include: Wire.h, Adafruit_TCS34725.h

#include <Wire.h>
#include <Adafruit_TCS34725.h>

Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!tcs.begin()) { Serial.println("TCS34725 not found"); while (1); }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  uint16_t colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  uint16_t lux = tcs.calculateLux(r, g, b);

  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.print(c);
  Serial.print("  CT: "); Serial.print(colorTemp); Serial.print("K");
  Serial.print("  Lux: "); Serial.println(lux);
}
