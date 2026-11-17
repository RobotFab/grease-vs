// snippet: sd_card
// tags: sd, sdcard, storage, file, log, csv, spi, datalogger, append, write, sensor, millis, analog
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: SD
// description: Log sensor data to SD card as CSV — append-on-open pattern.
//   SD.h (Arduino built-in): SD.begin(CS_PIN) uses SPI — always include SPI.h first.
//   SD.open(name, FILE_WRITE): opens for append if file exists, creates if not.
//   SD.open(name, FILE_READ): opens for reading only — FILE_WRITE will also read.
//   Returns a File object — NOT a FILE* (C stdio). Do NOT use fwrite/fclose.
//   Always call file.close() after writing — unflushed data is lost on power loss.
//   NOT: SD.write(filename, data) — no such method; open a File then call file.print().
//   NOT: SD.open() returning FILE* — it returns File (Arduino class), not C stdio pointer.
//   CS pin: Uno=10, Mega=53, ESP32 varies (often 5 or 15), Nano=10. Must match hardware wiring.
// prompts: SD card data logger, log CSV to SD, write sensor data file, datalogger SD card SPI, append CSV SD, log analogRead SD, SD card millis CSV, write file SD Arduino, SD.open FILE_WRITE, SD card sensor log, log data to file SD, SD SPI CS pin, SD.begin Arduino
// exact-include: SPI.h, SD.h

#include <SPI.h>  // must be included before SD.h — SD library depends on SPI
#include <SD.h>

const int SD_CS = 10;  // chip select pin — Uno/Nano=10, Mega=53; must match wiring
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  // SD.begin() initialises the SPI bus and selects the card via CS_PIN
  // Returns false if no card detected or card is incompatible (FAT16/FAT32 required)
  if (!SD.begin(SD_CS)) {  // SD: init card with CS pin; call once in setup
    Serial.println("SD init failed — check wiring and CS pin");
    while (1);
  }

  // Write CSV header only if file does not already exist (avoids duplicate headers on reboot)
  if (!SD.exists("log.csv")) {
    File f = SD.open("log.csv", FILE_WRITE);  // SD: open a file; returns a File (always check !file)
    if (f) {
      f.println("time_ms,voltage_V");
      f.close();  // flush and release — REQUIRED
    }
  }
  Serial.println("SD ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  float val = analogRead(A0) * (5.0f / 1023.0f);

  // Open for each write: slower but safe — no data lost if power fails between writes
  // For high-frequency logging, open once in setup() and flush periodically instead
  File logFile = SD.open("log.csv", FILE_WRITE);  // FILE_WRITE = append mode
  if (logFile) {
    logFile.print(now);
    logFile.print(",");
    logFile.println(val);
    logFile.close();  // flush to card — do not skip
  }
  Serial.print("logged: "); Serial.println(val);
}
