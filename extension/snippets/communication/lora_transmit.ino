// snippet: lora_transmit
// tags: lora, radiohead, spi, wireless, long-range, packet, transmit, iot
// boards: TBD
// library: RadioHead
// description: LoRa packet transmit using RadioHead RH_RF95 (SX1276)
// prompts: LoRa transmit, LoRa wireless, SX1276 radio, long range IoT, LoRa packet, RadioHead LoRa
// exact-include: SPI.h, RH_RF95.h

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS  10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
unsigned long prevMs = 0;
int packetNum = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  if (!rf95.init()) { Serial.println("LoRa init failed"); while (1); }
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
  Serial.println("LoRa TX ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  char buf[32];
  snprintf(buf, sizeof(buf), "pkt %d", packetNum++);
  rf95.send((uint8_t*)buf, strlen(buf));
  rf95.waitPacketSent();
  Serial.print("sent: "); Serial.println(buf);
}
