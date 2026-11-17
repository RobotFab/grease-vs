// snippet: sensor_imu_38
// tags: imu, accelerometer, gyroscope, i2c
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read ADXL345 accelerometer over I2C
// prompts: adxl345, accelerometer, declare, float, scope, initialize
// verified: compile-only
// source: kd-generated (id 38)
// exact-include: Wire.h

#include <Wire.h>

// ADXL345 I2C address (ALT ADDRESS pin low = 0x53)
#define ADXL345_ADDR 0x53

// ADXL345 registers
const byte DATAX0   = 0x32;  // first data register (X LSB)
const byte POWER_CTL = 0x2D; // power control register
const byte DATA_FORMAT = 0x31;

// Variables
float x, y, z;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // ADXL345 must be put into measurement mode — it starts in standby.
  // POWER_CTL bit 3 (MEASURE) = 1 enables measurement.
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(POWER_CTL);
  Wire.write(0x08);  // set MEASURE bit
  Wire.endTransmission();

  // Set data format to full resolution, ±2g range (0x08)
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(DATA_FORMAT);
  Wire.write(0x08);
  Wire.endTransmission();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;

    // Read 6 bytes starting from DATAX0 (0x32) — NOT 0x3B (that's MPU6050's register)
    Wire.beginTransmission(ADXL345_ADDR);
    Wire.write(DATAX0);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)ADXL345_ADDR, (uint8_t)6, (uint8_t)true);

    // ADXL345 sends LSB first (unlike MPU6050 which sends MSB first)
    int16_t ax = (int16_t)((Wire.read() | (Wire.read() << 8)));
    int16_t ay = (int16_t)((Wire.read() | (Wire.read() << 8)));
    int16_t az = (int16_t)((Wire.read() | (Wire.read() << 8)));

    // Convert raw to g — full resolution mode: 4 mg/LSB at ±2g
    x = ax * 0.0039;
    y = ay * 0.0039;
    z = az * 0.0039;

    // Print values
    Serial.print("X: "); Serial.print(x, 2);
    Serial.print(" Y: "); Serial.print(y, 2);
    Serial.print(" Z: "); Serial.println(z, 2);
  }
}
