// snippet: sensor_special_71
// tags: sensor, i2c, load-cell, mfrc522
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MFRC522
// description: Read MFRC522 RFID on SPI: SS=10, RST=9
// prompts: mfrc522, ss=10, rst=9, include, mfrc522, declare
// verified: compile-only
// source: kd-generated (id 71)
// exact-include: SPI.h, MFRC522.h

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN   10   // SDA / chip select
#define RST_PIN   9

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();  // MFRC522: init the RFID reader
  Serial.println("RC522 ready — tap a card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;  // MFRC522: true if an unread card is in range

  Serial.print("UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) Serial.print(":");
  }
  Serial.println();
  rfid.PICC_HaltA();
}
