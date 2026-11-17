// snippet: i2c_write_read
// tags: i2c, wire, twi, register, write, read, address, raw
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Low-level I2C register read/write using Wire — template for any I2C device
// prompts: I2C raw read write, Wire register access, I2C device template, write I2C register, read I2C register, low-level I2C
// exact-include: Wire.h

#include <Wire.h>

const uint8_t DEV_ADDR = 0x68;   // replace with your device address

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(DEV_ADDR, (uint8_t)1);
  return Wire.available() ? Wire.read() : 0xFF;
}

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  uint8_t whoAmI = readReg(0x75);   // WHO_AM_I register on MPU6050
  Serial.print("WHO_AM_I: 0x"); Serial.println(whoAmI, HEX);
}

void loop() {}
