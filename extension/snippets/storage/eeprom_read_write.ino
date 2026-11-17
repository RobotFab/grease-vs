// snippet: eeprom_read_write
// tags: eeprom, storage, byte, address, persistent, save, avr, mega, uno
// boards: avr, esp32, megaavr, romi32u4, rp2040
// library: EEPROM
// description: Read and write individual bytes and values to EEPROM with wear protection
// prompts: EEPROM read write byte, save to EEPROM, persistent byte storage AVR, EEPROM address value, non-volatile memory
// exact-include: EEPROM.h

#include <EEPROM.h>

// ESP32 and RP2040 use flash-emulated EEPROM: you MUST call begin(size)
// before get/put/write and commit() after writes, or data is never persisted.
// AVR's native EEPROM needs neither — begin()/commit() are no-ops there.
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #define EEPROM_SIZE 64  // enough for a few values
#endif

void writeFloat(int addr, float val) {
  EEPROM.put(addr, val);  // EEPROM: write any type/struct (handles size)
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  EEPROM.commit();  // flush to flash — without this, put() is lost on power-off
#endif
}

float readFloat(int addr) {
  float v;
  EEPROM.get(addr, v);  // EEPROM: read any type/struct
  return v;
}

void setup() {
  Serial.begin(9600);

#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  EEPROM.begin(EEPROM_SIZE);  // required on flash-emulated EEPROM cores
#endif

  // write only if value changed (saves write cycles)
  float newVal = 3.14f;
  float stored = readFloat(0);
  if (stored != newVal) {
    writeFloat(0, newVal);
    Serial.println("EEPROM updated");
  }

  Serial.print("stored: "); Serial.println(readFloat(0));

  // raw byte access
  EEPROM.write(4, 42);
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  EEPROM.commit();  // persist the byte write to flash
#endif
  Serial.print("byte at 4: "); Serial.println(EEPROM.read(4));
}

void loop() {}
