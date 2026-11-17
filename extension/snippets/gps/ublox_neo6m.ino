// snippet: ublox_neo6m
// tags: gps, neo6m, neo8m, nmea, tinygps, latitude, longitude, serial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: TinyGPS++
// description: Parse GPS NMEA from u-blox NEO-6M/8M using TinyGPS++ over SoftwareSerial
// prompts: u-blox NEO-6M GPS, NEO-8M GPS module, TinyGPS++ parsing, NMEA GPS serial, GPS latitude longitude coordinates
// exact-include: TinyGPSPlus.h, SoftwareSerial.h

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(4, 3);  // RX=4, TX=3
TinyGPSPlus gps;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("Waiting for GPS fix...");
}

void loop() {
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  if (gps.location.isValid()) {
    Serial.print("Lat: ");  Serial.print(gps.location.lat(), 6);
    Serial.print("  Lon: "); Serial.print(gps.location.lng(), 6);
    Serial.print("  Alt: "); Serial.print(gps.altitude.meters(), 1);
    Serial.print(" m  Sats: "); Serial.println(gps.satellites.value());
  } else {
    Serial.println("no fix");
  }
}
