// snippet: spi_basic
// tags: spi, mosi, miso, sck, cs, chip-select, transfer, raw
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Raw SPI byte transfer — template for any SPI peripheral
// prompts: SPI raw transfer, SPI peripheral template, SPI byte exchange, raw SPI communication, SPI bus transaction
// exact-include: SPI.h

#include <SPI.h>

const int CS_PIN = 10;

uint8_t spiTransfer(uint8_t val) {
  digitalWrite(CS_PIN, LOW);
  uint8_t result = SPI.transfer(val);
  digitalWrite(CS_PIN, HIGH);
  return result;
}

void setup() {
  Serial.begin(9600);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  uint8_t resp = spiTransfer(0x9F);   // READ ID command on many devices
  Serial.print("response: 0x"); Serial.println(resp, HEX);
  delay(500);
}
