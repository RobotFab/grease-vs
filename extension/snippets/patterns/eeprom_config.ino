// snippet: eeprom_config
// tags: eeprom, storage, config, persistent, save, load, struct, nvs
// boards: avr, esp32, megaavr, romi32u4, rp2040
// library: EEPROM
// description: Save and load a config struct to EEPROM — survives power cycles
// prompts: save config EEPROM, persistent settings storage, EEPROM struct save load, survive power cycle config, non-volatile config
// exact-include: EEPROM.h

#include <EEPROM.h>

struct Config {
  float threshold;
  int   mode;
  char  name[16];
  uint32_t magic;  // sentinel to detect uninitialized EEPROM
};

const uint32_t MAGIC = 0xBEEF1234;
const int EEPROM_ADDR = 0;

Config cfg;

void saveConfig() {
  EEPROM.put(EEPROM_ADDR, cfg);  // EEPROM: write any type/struct (handles size)
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  EEPROM.commit();               // flash-emulated EEPROM needs commit() on ESP32/RP2040
#endif
}

bool loadConfig() {
  EEPROM.get(EEPROM_ADDR, cfg);  // EEPROM: read any type/struct
  return cfg.magic == MAGIC;
}

void defaultConfig() {
  cfg.threshold = 3.3f;
  cfg.mode = 1;
  strncpy(cfg.name, "default", sizeof(cfg.name));
  cfg.magic = MAGIC;
}

void setup() {
  Serial.begin(9600);
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  EEPROM.begin(sizeof(Config));  // flash-emulated EEPROM must be initialised on ESP32/RP2040
#endif
  if (!loadConfig()) {
    Serial.println("No config found — using defaults");
    defaultConfig();
    saveConfig();
  }
  Serial.print("threshold: "); Serial.println(cfg.threshold);
  Serial.print("mode: "); Serial.println(cfg.mode);
}

void loop() {}
