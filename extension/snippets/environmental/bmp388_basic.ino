// snippet: bmp388_basic
// tags: environmental, bmp388, pressure, temperature, altitude, barometric, adafruit, i2c
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit BMP3XX
// description: Read temperature, pressure, and estimated altitude from BMP388
// prompts: BMP388, barometric pressure altitude, pressure sensor, temperature pressure, altitude estimation, BMP388 I2C
// exact-include: Wire.h, Adafruit_BMP3XX.h

#include <Wire.h>
#include <Adafruit_BMP3XX.h>

Adafruit_BMP3XX bmp;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin_I2C(0x77)) { Serial.println("BMP388 not found"); while (1); }

  // BMP388 API uses BMP3_OVERSAMPLING_*X constants (NOT BMP3XX_* and NOT deprecated OSRS_*).
  // BMP3_OVERSAMPLING_8X = best accuracy; BMP3_OVERSAMPLING_2X for lower power.
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  if (!bmp.performReading()) { Serial.println("BMP388 read failed"); return; }

  Serial.print("T: "); Serial.print(bmp.temperature); Serial.print("°C  ");
  Serial.print("P: "); Serial.print(bmp.pressure / 100.0); Serial.print(" hPa  ");
  Serial.print("Alt: "); Serial.print(bmp.readAltitude(1013.25)); Serial.println(" m");
}
