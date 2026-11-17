// snippet: pico_spi_dac
// tags: pico, rp2040, raspberry-pi, spi, dac, mcp4922, mcp4921, analog-output, digital-to-analog, 12-bit
// boards: rp2040
// library: SPI
// description: RPi Pico: SPI write to MCP4922 12-bit dual DAC (CS=pin 17) at 1 MHz.
//   On RP2040, SPI pin assignment is done via SPI.setMOSI/setMISO/setSCK BEFORE SPI.begin().
//   Default SPI0 pins: MOSI=GP3, MISO=GP4, SCK=GP2. SPI1 pins differ.
//   MCP4922 command word (16-bit): [A/B][BUF][GA][SHDN][D11..D0]
//     bit15: 0=channel A, 1=channel B
//     bit14: 0=unbuffered Vref, 1=buffered
//     bit13: 0=2x gain (Vout=2*Vref*D/4096), 1=1x gain (Vout=Vref*D/4096)
//     bit12: 1=output active (SHDN=1), 0=output off
//     bits 11-0: 12-bit data value
//   NOT: analogWrite() — RP2040 has no true DAC; analogWrite drives PWM only.
//   NOT: dacWrite() — that is the ESP32 API (8-bit, pins 25/26 only).
// prompts: SPI DAC Pico, MCP4922 RP2040, analog output Pico SPI, 12-bit DAC Raspberry Pi Pico, MCP4921 Pico, DAC SPI write Pico, digital to analog RP2040, write voltage Pico SPI
// not-tags: esp32, avr, analogWrite, dacWrite
// exact-include: SPI.h

#include <SPI.h>

const int CS_PIN = 17;  // chip select — active LOW; any GPIO works on RP2040

// Build MCP4922 16-bit command word for channel A, 1x gain, output enabled
uint16_t makeMCP4922Word(uint16_t value12bit, bool channelB = false) {
  // bits: [A/B=0][BUF=0][GA=1 (1x)][SHDN=1 (active)] | [12-bit data]
  uint16_t cmd = 0x3000;           // 0b0011_0000_0000_0000 = chan A, unbuffered, 1x gain, output on
  if (channelB) cmd |= 0x8000;     // bit15=1 → channel B
  cmd |= (value12bit & 0x0FFF);    // mask to 12 bits and OR into lower nibble
  return cmd;
}

void writeDAC(uint16_t word) {
  digitalWrite(CS_PIN, LOW);       // assert chip select
  // SPI.transfer16 sends MSB first — required by MCP4922 protocol
  SPI.transfer16(word);
  digitalWrite(CS_PIN, HIGH);      // latch output: DAC updates on CS rising edge
}

void setup() {
  Serial.begin(115200);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);      // deselect before SPI.begin()

  // RP2040: assign SPI pins before SPI.begin() — default SPI0: MOSI=GP3, SCK=GP2
  // SPI.setMOSI(3); SPI.setSCK(2);  // uncomment to override defaults
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  // 1 MHz, mode 0
}

unsigned long prevMs = 0;
uint16_t val = 0;

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 10) {
    prevMs = now;
    writeDAC(makeMCP4922Word(val));
    val = (val + 100) % 4096;  // sweep 0–4095 (12-bit full scale)
  }
}
