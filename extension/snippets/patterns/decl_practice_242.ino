// snippet: decl_practice_242
// tags: globals, declare, millis, nonblocking, mfrc522
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MFRC522
// description: Declare MFRC522 rfid(10, 9) at global scope
// prompts: declare, mfrc522, rfid(10, scope, begin(), pcd_init()
// verified: compile-only
// source: kd-generated (id 242)
// exact-include: SPI.h, MFRC522.h

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();  // MFRC522: init the RFID reader
  Serial.println("RFID reader initialized");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {  // MFRC522: true if an unread card is in range
    Serial.print("UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
      Serial.print(rfid.uid.uidByte[i], HEX);
      if (i < rfid.uid.size - 1) Serial.print(":");
    }
    Serial.println();
    rfid.PICC_HaltA();
  }
}
