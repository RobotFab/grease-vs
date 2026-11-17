// snippet: combined_224
// tags: combined, multi-sensor, millis, mfrc522, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: MFRC522, Servo
// description: RFID-triggered servo: MFRC522 SS=10 RST=9
// prompts: rfid-triggered, servo, mfrc522, ss=10, rst=9, known
// verified: compile-only
// source: kd-generated (id 224)
// exact-include: SPI.h, MFRC522.h, Servo.h

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN   10
#define RST_PIN   9
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;
int16_t ax, ay, az;
const byte knownUID[] = {0xDE, 0xAD, 0xBE, 0xEF};  // Replace with actual UID

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();  // MFRC522: init the RFID reader
  myServo.attach(9);
  Serial.println("RFID and Servo ready");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;  // MFRC522: true if an unread card is in range

  bool isKnownCard = true;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] != knownUID[i]) {
      isKnownCard = false;
      break;
    }
  }

  if (isKnownCard) {
    for (int angle = 0; angle <= 180; angle++) {
      myServo.write(angle);
      delay(15);
    }
    for (int angle = 180; angle >= 0; angle--) {
      myServo.write(angle);
      delay(15);
    }
  }

  rfid.PICC_HaltA();
}
