// snippet: tmp117_precision_temp
// tags: temperature, tmp117, precision, i2c, sparkfun, high-accuracy, calibrated
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: SparkFun TMP117
// description: High-accuracy ±0.1°C temperature from TMP117 using SparkFun library
// prompts: TMP117 precision temperature, high accuracy thermometer, 0.1 degree accuracy, calibrated temperature sensor, SparkFun TMP117
// exact-include: Wire.h, SparkFun_TMP117.h

#include <Wire.h>
#include <SparkFun_TMP117.h>

TMP117 sensor;
unsigned long prevMs = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if (!sensor.begin()) { Serial.println("TMP117 not found"); while (1); }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 250) return;
  prevMs = now;

  if (sensor.dataReady()) {
    float tempC = sensor.readTempC();
    Serial.print("T: "); Serial.print(tempC, 4); Serial.println("°C");
  }
}
