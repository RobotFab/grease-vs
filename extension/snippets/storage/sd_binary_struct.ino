// snippet: sd_binary_struct
// tags: sd, sdcard, storage, file, binary, struct, packed, log, write, sensor, compact, high-speed
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: SD
// description: Log packed binary structs to SD — more compact and faster than CSV text.
//   file.write((uint8_t*)&rec, sizeof(rec)): writes raw bytes of the struct directly.
//   Struct must be __attribute__((packed)) to avoid compiler padding bytes between fields.
//   Without packed: sizeof may be larger than expected due to alignment — corrupt records on read.
//   Binary files are not human-readable but are smaller (e.g. 10 bytes/record vs ~30 chars CSV).
//   To read back: open with FILE_READ, call file.read((uint8_t*)&rec, sizeof(rec)) in a loop.
//   NOT: file.print(rec) — no overload for structs; use file.write() with a byte pointer cast.
//   NOTE: binary format is platform-specific (endianness, int size). AVR = little-endian, 2-byte int.
//         Reading on a PC requires matching the struct layout exactly.
// prompts: SD binary log, SD packed struct, write struct to SD, binary data logger SD, SD file.write bytes, compact SD log, high speed SD logging, SD raw bytes struct, SD binary record, log struct SD card
// exact-include: SPI.h, SD.h

#include <SPI.h>
#include <SD.h>

const int SD_CS = 10;

// Packed struct: no padding bytes added by compiler — sizeof = 4+2+2+2+2 = 12 bytes exactly
struct __attribute__((packed)) LogRecord {
  uint32_t timeMs;   // millis() timestamp
  int16_t  ax;       // accel X raw (or any int16 sensor value)
  int16_t  ay;
  int16_t  az;
  int16_t  gz;       // gyro Z raw
};

unsigned long prevMs = 0;
const unsigned long INTERVAL_MS = 20;  // 50 Hz

void setup() {
  Serial.begin(9600);
  if (!SD.begin(SD_CS)) {  // SD: init card with CS pin; call once in setup
    Serial.println("SD init failed");
    while (1);
  }
  // Binary log file — open once, keep open for fast repeated writes
  // FILE_WRITE appends if file exists; use SD.remove("data.bin") first for a fresh log
  Serial.println("SD ready — binary logging at 50 Hz");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL_MS) return;
  prevMs = now;

  LogRecord rec;
  rec.timeMs = now;
  rec.ax = analogRead(A0) - 512;  // placeholder — replace with real sensor reads
  rec.ay = analogRead(A1) - 512;
  rec.az = analogRead(A2) - 512;
  rec.gz = 0;

  // Open, write one record, close — safe but slow. For >50 Hz, open once in setup() instead.
  File f = SD.open("data.bin", FILE_WRITE);  // SD: open a file; returns a File (always check !file)
  if (f) {
    // Cast struct pointer to byte pointer for raw write
    f.write((const uint8_t*)&rec, sizeof(rec));  // writes exactly sizeof(LogRecord) bytes
    f.close();  // SD: flush + close the file (always call it!)
  }
}
