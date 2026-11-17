// snippet: rfid_rc522
// tags: rfid, rc522, spi, nfc, uid, card, tag
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MFRC522
// description: Read RFID card/tag UID using an RC522 module over SPI.
//   Wiring (Uno/Nano): SDA→SS_PIN, SCK→13, MOSI→11, MISO→12, RST→RST_PIN, 3.3V, GND.
//   ESP32 default SPI: SDA→5, SCK→18, MOSI→23, MISO→19. Adjust SS_PIN/RST_PIN below.
// prompts: RFID RC522, read card UID, NFC tag reader, MFRC522 SPI, access control RFID, card scanner
//
// ---- CONFIGURE ----
// exact-include: SPI.h, MFRC522.h
#define SS_PIN   10   // SDA / chip select
#define RST_PIN   9
// -------------------

#include <SPI.h>
#include <MFRC522.h>

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
