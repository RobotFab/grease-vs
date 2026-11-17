// snippet: sensor_special_70
// tags: sensor, i2c, load-cell, softwareserial, tinygps++
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: SoftwareSerial, TinyGPS++
// description: Read NEO-6M GPS over SoftwareSerial on rx=2, tx=3
// prompts: neo-6m, softwareserial, include, tinygps++, declare, tinygpsplus
// verified: compile-only
// source: kd-generated (id 70)
// exact-include: SoftwareSerial.h, TinyGPS++.h

#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial ss(2, 3);  // RX=2, TX=3
TinyGPSPlus gps;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("GPS waiting for fix...");
}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isValid()) {
        Serial.print("Lat: "); Serial.print(gps.location.lat(), 6);
        Serial.print("  Lon: "); Serial.print(gps.location.lng(), 6);
        Serial.println();
      }
    }
  }

  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;
}
