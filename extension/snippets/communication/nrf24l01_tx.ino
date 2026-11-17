// snippet: nrf24l01_tx
// tags: nrf24l01, rf24, radio, wireless, spi, transmit, send, 2.4ghz
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: RF24
// description: Transmit a float value wirelessly with nRF24L01 using RF24 library
// prompts: nRF24L01 transmit, RF24 wireless, 2.4GHz radio, send data wireless, Nordic radio, NRF24 transmitter
// exact-include: SPI.h, RF24.h

#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE=7, CSN=8
const byte address[6] = "00001";
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  float payload = analogRead(A0) * (5.0 / 1023.0);
  bool ok = radio.write(&payload, sizeof(payload));
  Serial.print("send "); Serial.print(payload); Serial.println(ok ? " OK" : " FAIL");
}
