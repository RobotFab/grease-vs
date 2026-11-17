// snippet: apds9960_color
// tags: color, rgb, apds9960, sparkfun, i2c, ambient-light, light-sensor
// not-tags: adafruit, Adafruit_APDS9960, arduino-apds9960, gesture, swipe
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: SparkFun APDS9960 RGB and Gesture Sensor
// description: Poll RGBA color channels from APDS9960 using the SparkFun library.
//   Init: apds.init() then apds.enableLightSensor(false).
//   Read: apds.readRedLight(r), readGreenLight(g), readBlueLight(b), readAmbientLight(c) — ALL by reference, uint16_t.
//   NOT Adafruit_APDS9960 — that library uses apds.begin() and different method names.
//   Sensor runs on 3.3V — do NOT connect directly to 5V Arduino pins without a level shifter.
// prompts: APDS9960 color sensor, RGB color reading, ambient light APDS9960, SparkFun APDS9960 color, RGBA light sensor, color detection I2C, SparkFun_APDS9960
// exact-include: Wire.h, SparkFun_APDS9960.h

#include <Wire.h>
#include <SparkFun_APDS9960.h>

SparkFun_APDS9960 apds;
uint16_t r = 0, g = 0, b = 0, c = 0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!apds.init()) { Serial.println("APDS9960 not found"); while (1); }
  apds.enableLightSensor(false);  // false = polling, no interrupt
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  // Read by reference — return value is bool (true = success)
  apds.readRedLight(r);
  apds.readGreenLight(g);
  apds.readBlueLight(b);
  apds.readAmbientLight(c);

  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);
}
