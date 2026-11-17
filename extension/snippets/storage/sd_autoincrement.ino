// snippet: sd_autoincrement
// tags: sd, sdcard, storage, file, log, csv, autoincrement, filename, session, boot, LOG_NNN, new-file
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: SD
// description: On each boot, create a new log file with an auto-incrementing name (LOG000.CSV, LOG001.CSV, ...).
//   Pattern: scan for the first filename that does NOT exist — that becomes this session's log file.
//   8.3 filename limit on FAT: names must be ≤8 chars + ≤3 char extension. "LOG000.CSV" = valid.
//   NOT: SD.open("LOG_%d.CSV", n) — SD.h does not support format strings; build with sprintf or snprintf.
//   NOT: SD.listFiles() — no such method in Arduino SD.h; use SD.exists() to probe filenames instead.
//   Max files: FAT16 root dir limit is 512 entries; FAT32 has no practical limit.
// prompts: SD auto increment filename, new log file each boot, SD session file, LOG000 CSV SD, auto name SD file, SD boot new file, increment log file name, SD card new session, SD auto file number, SD LOG_NNN, create numbered file SD
// exact-include: SPI.h, SD.h

#include <SPI.h>
#include <SD.h>

const int SD_CS = 10;
char logName[13];       // 8.3 format: "LOG000.CSV" = 10 chars + null = 11; 13 is safe
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!SD.begin(SD_CS)) {  // SD: init card with CS pin; call once in setup
    Serial.println("SD init failed");
    while (1);
  }

  // Find first unused LOG_NNN.CSV filename
  for (int i = 0; i <= 999; i++) {
    snprintf(logName, sizeof(logName), "LOG%03d.CSV", i);  // e.g. "LOG007.CSV"
    if (!SD.exists(logName)) break;  // this name is free — use it
    if (i == 999) {
      Serial.println("No free log slot (LOG000–LOG999 all exist)");
      while (1);
    }
  }

  // Write CSV header for this session
  File f = SD.open(logName, FILE_WRITE);  // SD: open a file; returns a File (always check !file)
  if (f) {
    f.println("time_ms,value");
    f.close();  // SD: flush + close the file (always call it!)
  }
  Serial.print("Logging to: "); Serial.println(logName);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  File f = SD.open(logName, FILE_WRITE);  // append to this session's file
  if (f) {
    f.print(now); f.print(","); f.println(analogRead(A0));
    f.close();  // SD: flush + close the file (always call it!)
  }
}
