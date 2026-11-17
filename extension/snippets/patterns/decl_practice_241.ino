// snippet: decl_practice_241
// tags: globals, declare, millis, nonblocking, tinygpsplus, softwareserial
// boards: avr, megaavr, rp2040
// not-tags: romi32u4
// library: TinyGPSPlus, SoftwareSerial
// description: Declare TinyGPSPlus gps and SoftwareSerial ss(2,3) at global scope
// prompts: declare, tinygpsplus, softwareserial, scope, read(), encode()
// verified: compile-only
// source: kd-generated (id 241)
// exact-include: TinyGPSPlus.h, SoftwareSerial.h

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

SoftwareSerial ss(2, 3);  // RX=2, TX=3
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("GPS reader initialized");
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  if (gps.location.isUpdated()) {
    Serial.print("Lat: "); Serial.print(gps.location.lat(), 6);
    Serial.print("  Lon: "); Serial.println(gps.location.lng(), 6);
  }
}
