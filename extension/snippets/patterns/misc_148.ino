// snippet: misc_148
// tags: misc, millis, nonblocking, eeprom
// boards: avr, esp32, megaavr, romi32u4, rp2040
// library: EEPROM
// description: EEPROM storage: on SAVE <value> command, store calibration float to EEPROM using EEPROM.put()
// prompts: eeprom, storage, command, store, calibration, float
// verified: compile-only
// source: kd-generated (id 148)
// exact-include: EEPROM.h

#include <EEPROM.h>

float calValue = 0.0f;
const int MAGIC_ADDR = 0;   // magic byte at address 0
const int CAL_ADDR   = 1;   // float at address 1
const byte MAGIC_BYTE = 0xAB;

void saveCalibration(float value) {
  EEPROM.put(CAL_ADDR, value);  // EEPROM: write any type/struct (handles size)
  EEPROM.write(MAGIC_ADDR, MAGIC_BYTE);  // mark as valid
}

bool loadCalibration() {
  byte magic;
  EEPROM.get(MAGIC_ADDR, magic);  // EEPROM: read any type/struct
  if (magic == MAGIC_BYTE) {
    EEPROM.get(CAL_ADDR, calValue);  // EEPROM: read any type/struct
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  if (!loadCalibration()) {
    Serial.println("No calibration found");
    calValue = 0.0f;
  } else {
    Serial.print("Loaded calibration value: ");
    Serial.println(calValue);
  }
  Serial.println("Send: SAVE <value>");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SAVE")) {
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        float newCalValue = command.substring(spaceIdx + 1).toFloat();
        saveCalibration(newCalValue);
        calValue = newCalValue;
        Serial.print("Calibration saved: ");
        Serial.println(calValue);
      }
    }
  }
}
