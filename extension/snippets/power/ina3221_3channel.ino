// snippet: ina3221_3channel
// tags: power, current, voltage, ina3221, 3-channel, i2c, monitor, adafruit
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: Adafruit INA3221
// description: Measure voltage and current on 3 channels simultaneously with INA3221
// prompts: INA3221 power monitor, 3-channel current voltage, triple rail power measurement, multi-channel power sensor
// exact-include: Wire.h, Adafruit_INA3221.h

#include <Wire.h>
#include <Adafruit_INA3221.h>

Adafruit_INA3221 ina3221;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!ina3221.begin()) { Serial.println("INA3221 not found"); while (1); }
  for (int ch = 0; ch < 3; ch++) ina3221.enableChannel(ch);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  for (int ch = 0; ch < 3; ch++) {
    Serial.print("CH"); Serial.print(ch + 1);
    Serial.print(": "); Serial.print(ina3221.getBusVoltage(ch), 3); Serial.print(" V  ");
    Serial.print(ina3221.getCurrentAmps(ch) * 1000.0, 1); Serial.print(" mA  ");
  }
  Serial.println();
}
