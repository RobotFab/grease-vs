// snippet: nrf24l01_rx
// tags: nrf24l01, rf24, radio, wireless, spi, receive, listen, 2.4ghz
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: RF24
// description: Receive a float wirelessly with nRF24L01 — companion to nrf24l01_tx
// prompts: nRF24L01 receive, RF24 wireless receive, 2.4GHz radio receive, Nordic radio receiver, NRF24 receiver
// exact-include: SPI.h, RF24.h

#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    float payload;
    radio.read(&payload, sizeof(payload));
    Serial.print("received: ");
    Serial.println(payload);
  }
}
