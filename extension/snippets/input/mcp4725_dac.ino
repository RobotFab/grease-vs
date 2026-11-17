// snippet: mcp4725_dac
// tags: dac, mcp4725, analog-output, voltage, i2c, adafruit, waveform, signal
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit MCP4725
// description: Generate analog voltage (0-5V) with 12-bit MCP4725 DAC — sawtooth wave example
// prompts: MCP4725 DAC, analog voltage output, digital to analog converter I2C, waveform generator, 12-bit DAC sawtooth
// exact-include: Wire.h, Adafruit_MCP4725.h

#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;
uint16_t dacVal = 0;

void setup() {
  Serial.begin(9600);
  dac.begin(0x60);
}

void loop() {
  dac.setVoltage(dacVal, false);  // false = don't save to EEPROM
  dacVal = (dacVal + 64) & 0xFFF; // sawtooth 0-4095, 64 steps
  delay(5);
}
