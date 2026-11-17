// snippet: sensor_special_72
// tags: sensor, i2c, load-cell, mfrc522
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MFRC522
// description: RFID access control: MFRC522 on SS=10, RST=9
// prompts: access, control, mfrc522, ss=10, rst=9, declare
// verified: compile-only
// source: kd-generated (id 72)
// exact-include: SPI.h, MFRC522.h

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN   10   // SDA / chip select
#define RST_PIN   9

MFRC522 rfid(SS_PIN, RST_PIN);

const byte allowedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();  // MFRC522: init the RFID reader
  Serial.println("RFID access control — tap a card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;  // MFRC522: true if an unread card is in range

  bool isAllowed = true;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      isAllowed = false;
      break;
    }
  }

  if (isAllowed) {
    Serial.println("GRANTED");
  } else {
    Serial.println("DENIED");
  }

  Serial.print("Timestamp: ");
  Serial.println(millis());
  rfid.PICC_HaltA();
}
