// snippet: gps_neom8n
// tags: gps, neo6m, neo8m, neom8n, uart, tinygps, latitude, longitude, nmea
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: TinyGPS++
// description: Parse GPS NMEA data from NEO-6M/8M module using TinyGPS++ and SoftwareSerial
// prompts: GPS NEO-6M, GPS NEO-8M, TinyGPS++, latitude longitude GPS, GPS coordinates, NMEA parsing, GPS module
// exact-include: SoftwareSerial.h, TinyGPS++.h

#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(4, 3);  // RX=4, TX=3
TinyGPSPlus gps;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("GPS waiting for fix…");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  if (gps.location.isValid()) {
    Serial.print("lat: ");    Serial.print(gps.location.lat(), 6);
    Serial.print(" lng: ");   Serial.print(gps.location.lng(), 6);
    Serial.print(" alt: ");   Serial.println(gps.altitude.meters());
  } else {
    Serial.println("no fix");
  }
}
