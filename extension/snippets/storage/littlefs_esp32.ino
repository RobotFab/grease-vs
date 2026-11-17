// snippet: littlefs_esp32
// tags: littlefs, spiffs, esp32, filesystem, storage, file, json, flash
// boards: esp32
// library: none
// description: Read and write files on ESP32 flash using LittleFS (preferred over SPIFFS)
// prompts: LittleFS ESP32 filesystem, ESP32 flash file storage, read write file ESP32, SPIFFS replacement LittleFS
// exact-include: LittleFS.h

#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  if (!LittleFS.begin(true)) {  // true = format if mount fails
    Serial.println("LittleFS mount failed");
    while (1);
  }

  // write
  File f = LittleFS.open("/config.txt", "w");
  if (f) { f.println("threshold=3.5"); f.println("mode=auto"); f.close(); }

  // read back
  f = LittleFS.open("/config.txt", "r");
  if (f) {
    while (f.available()) Serial.write(f.read());
    f.close();
  }

  // list root
  File root = LittleFS.open("/");
  File entry = root.openNextFile();
  while (entry) {
    Serial.print("  "); Serial.print(entry.name());
    Serial.print("  "); Serial.println(entry.size());
    entry = root.openNextFile();
  }
}

void loop() {}
