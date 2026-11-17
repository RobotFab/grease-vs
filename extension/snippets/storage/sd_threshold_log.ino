// snippet: sd_threshold_log
// tags: sd, sdcard, storage, file, log, csv, threshold, conditional, trigger, event, analog, sensor
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: SD
// description: Log to SD only when a sensor value exceeds a threshold — event-driven logging.
//   Reduces SD writes to only meaningful events; avoids filling card with idle baseline data.
//   FILE_WRITE always appends — no seek/overwrite needed for simple event logs.
//   Pattern: read sensor every poll interval; write only when condition is met.
//   NOT: SD.open() inside ISR — SD SPI is not interrupt-safe; always log from loop().
// prompts: SD threshold log, log to SD when value exceeds, conditional SD logging, event driven SD, SD only when sensor high, trigger SD write threshold, analog threshold SD card, SD log on event, SD card conditional write, log above threshold SD
// exact-include: SPI.h, SD.h

#include <SPI.h>
#include <SD.h>

const int SD_CS    = 10;
const int THRESHOLD = 600;   // log when analogRead(A0) > this value (0–1023)
unsigned long prevMs = 0;
const unsigned long POLL_MS = 100;  // check sensor every 100 ms

void setup() {
  Serial.begin(9600);
  if (!SD.begin(SD_CS)) {  // SD: init card with CS pin; call once in setup
    Serial.println("SD init failed");
    while (1);
  }
  // Write header once (only if file is new)
  if (!SD.exists("events.csv")) {
    File f = SD.open("events.csv", FILE_WRITE);  // SD: open a file; returns a File (always check !file)
    if (f) { f.println("time_ms,value"); f.close(); }  // SD: flush + close the file (always call it!)
  }
  Serial.println("SD ready — logging events above threshold");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < POLL_MS) return;
  prevMs = now;

  int val = analogRead(A0);

  if (val > THRESHOLD) {
    // Only open SD and write when the condition is met — saves write cycles
    File f = SD.open("events.csv", FILE_WRITE);  // SD: open a file; returns a File (always check !file)
    if (f) {
      f.print(now); f.print(","); f.println(val);
      f.close();  // flush immediately after each event
    }
    Serial.print("event: "); Serial.println(val);
  }
}
