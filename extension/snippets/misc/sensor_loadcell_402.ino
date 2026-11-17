// snippet: sensor_loadcell_402
// tags: eeprom, hx711
// boards: avr, esp32, megaavr, romi32u4, rp2040
// library: EEPROM, HX711
// description: Save the calibration factor for the HX711 (DOUT=3, CLK=4) to EEPROM (address 0, float)
// prompts: calibration, factor, hx711, (dout=3, clk=4), eeprom
// verified: compile-only
// source: kd-generated (id 402)
// exact-include: EEPROM.h, HX711.h

#include <EEPROM.h>
#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
float calibrationFactor = 0.0f;

// Magic byte at addr 0 validates that EEPROM has been written;
// float stored at addr 4 (4-byte aligned).
const int MAGIC_ADDR = 0;
const int CAL_ADDR   = 4;
const byte MAGIC_BYTE = 0xAB;

void saveCalibration(float value) {
  EEPROM.put(CAL_ADDR, value);  // EEPROM: write any type/struct (handles size)
  EEPROM.write(MAGIC_ADDR, MAGIC_BYTE);  // mark as valid
  #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
    EEPROM.commit();  // ESP32/RP2040: flash-backed EEPROM needs commit()
  #endif
}

bool loadCalibration() {
  byte magic;
  EEPROM.get(MAGIC_ADDR, magic);  // EEPROM: read any type/struct
  if (magic == MAGIC_BYTE) {
    EEPROM.get(CAL_ADDR, calibrationFactor);  // EEPROM: read any type/struct
    return true;
  }
  return false;  // not yet calibrated
}

void setup() {
  Serial.begin(9600);
  #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
    EEPROM.begin(64);  // ESP32/RP2040: flash-backed EEPROM needs begin()
  #endif
  if (!loadCalibration()) {
    Serial.println("No calibration found — using default");
    calibrationFactor = 2280.0f;  // default calibration factor
    saveCalibration(calibrationFactor);
  }
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(calibrationFactor);  // HX711: calibration factor (units per raw count)
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'c') {
      Serial.println("Enter new calibration factor:");
      unsigned long tIn = millis();
      while (!Serial.available()) {  // wait for input with timeout
        if (millis() - tIn > 10000) {
          Serial.println("Calibration timed out");
          return;
        }
      }
      calibrationFactor = Serial.parseFloat();
      saveCalibration(calibrationFactor);
      scale.set_scale(calibrationFactor);  // HX711: calibration factor (units per raw count)
      Serial.println("Calibration updated");
    }
  }
}
