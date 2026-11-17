// snippet: sd_info
// tags: sd, sdcard, storage, file, list, directory, ls, free-space, size, exists, check, boot
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: SD
// description: On boot: verify SD card present, list files in root directory, print file sizes.
//   SD.open("/") opens the root directory as a File object — iterate with dir.openNextFile().
//   dir.openNextFile(): returns the next File in the directory; returns falsy when exhausted.
//   file.name(): returns the 8.3 filename string. file.size(): returns size in bytes (uint32_t).
//   file.isDirectory(): true for subdirectories.
//   NOT: SD.listFiles() — no such method in Arduino SD.h.
//   NOT: SD.totalSize() / SD.freeSize() — not available in standard Arduino SD.h; use SDFat library for that.
//   FAT volume type and card size require the SDFat library (not SD.h) for detailed info.
// prompts: SD card list files, SD directory listing, SD ls root, list SD files serial, SD file names serial, SD card check boot, SD open root directory, SD openNextFile, print SD files, SD file size, SD exists check, SD card info
// exact-include: SPI.h, SD.h

#include <SPI.h>
#include <SD.h>

const int SD_CS = 10;

void listDirectory(File dir, int indent = 0) {
  while (true) {
    File entry = dir.openNextFile();  // returns false/falsy when no more files
    if (!entry) break;

    // Print indentation for nested dirs
    for (int i = 0; i < indent; i++) Serial.print("  ");

    Serial.print(entry.name());       // 8.3 format filename
    if (entry.isDirectory()) {
      Serial.println("/");
      listDirectory(entry, indent + 1);  // recurse into subdirectory
    } else {
      Serial.print("  ");
      Serial.print(entry.size());     // size in bytes (uint32_t)
      Serial.println(" bytes");
    }
    entry.close();  // SD: flush + close the file (always call it!)
  }
}

void setup() {
  Serial.begin(9600);
  // Bounded wait for USB serial — without a timeout, this hangs forever
  // if no USB host is attached (e.g. battery-powered operation).
  while (!Serial && millis() < 3000);

  if (!SD.begin(SD_CS)) {  // SD: init card with CS pin; call once in setup
    Serial.println("SD init failed — no card or wrong CS pin");
    while (1);
  }
  Serial.println("SD card found. Root directory:");

  File root = SD.open("/");  // open root as directory
  listDirectory(root);
  root.close();  // SD: flush + close the file (always call it!)

  // Check for a specific file
  if (SD.exists("log.csv")) {
    Serial.println("log.csv exists");
  } else {
    Serial.println("log.csv not found");
  }
}

void loop() {}
