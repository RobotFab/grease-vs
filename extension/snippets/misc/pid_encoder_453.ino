// snippet: pid_encoder_453
// tags: eeprom
// boards: avr, esp32, megaavr, romi32u4, rp2040
// library: EEPROM
// description: Save and load PID gains (Kp, Ki, Kd as floats) to EEPROM
// prompts: gains, floats), eeprom, last-used, gains, current
// verified: compile-only
// source: kd-generated (id 453)
// exact-include: EEPROM.h

#include <EEPROM.h>

struct PIDGains {
  float Kp;
  float Ki;
  float Kd;
  uint32_t magic;  // sentinel to detect uninitialized EEPROM
};

const uint32_t MAGIC = 0xBEEF1234;
const int EEPROM_ADDR = 0;

PIDGains pidGains;

void saveGains() {
  EEPROM.put(EEPROM_ADDR, pidGains);  // EEPROM: write any type/struct (handles size)
  #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
    EEPROM.commit();  // ESP32/RP2040: flash-backed EEPROM needs commit()
  #endif
}

bool loadGains() {
  EEPROM.get(EEPROM_ADDR, pidGains);  // EEPROM: read any type/struct
  return pidGains.magic == MAGIC;
}

void defaultGains() {
  pidGains.Kp = 1.0f;
  pidGains.Ki = 0.1f;
  pidGains.Kd = 0.01f;
  pidGains.magic = MAGIC;
}

void setup() {
  Serial.begin(9600);
  #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
    EEPROM.begin(sizeof(PIDGains));  // ESP32/RP2040: flash-backed EEPROM needs begin()
  #endif
  if (!loadGains()) {
    Serial.println("No gains found — using defaults");
    defaultGains();
    saveGains();
  }
  Serial.print("Loaded gains: Kp="); Serial.println(pidGains.Kp);
  Serial.print("Ki="); Serial.println(pidGains.Ki);
  Serial.print("Kd="); Serial.println(pidGains.Kd);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'w') {
      saveGains();
      Serial.println("Gains saved to EEPROM");
    }
  }
}
