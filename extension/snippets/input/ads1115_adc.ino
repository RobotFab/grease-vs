// snippet: ads1115_adc
// tags: adc, ads1115, 16-bit, i2c, analog, voltage, adafruit, high-res
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit ADS1X15
// description: 16-bit ADC readings from ADS1115 with programmable gain.
//   CORRECT header: #include <Adafruit_ADS1X15.h>  (the X15 covers both ADS1015 and ADS1115)
//   NOT: #include <Adafruit_ADS1115.h> — that header does not exist; the file is Adafruit_ADS1X15.h
// prompts: ADS1115 16-bit ADC, high resolution analog input, external ADC I2C, precise voltage reading, 4-channel external ADC
// exact-include: Wire.h, Adafruit_ADS1X15.h

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!ads.begin(0x48)) {
    Serial.println("ADS1115 not found");
    while (1);
  }
  ads.setGain(GAIN_ONE);  // ±4.096V range
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  int16_t raw    = ads.readADC_SingleEnded(0);
  float voltage  = ads.computeVolts(raw);
  Serial.print("ch0: "); Serial.print(raw);
  Serial.print(" v: "); Serial.println(voltage);
}
