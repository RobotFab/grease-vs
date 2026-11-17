// snippet: pa1010d_i2c_gps
// tags: gps, pa1010d, i2c, adafruit, nmea, latitude, longitude, mini-gps
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit GPS
// description: I2C GPS using Adafruit PA1010D (MTK3333) module + Adafruit GPS library
// prompts: PA1010D GPS I2C, Adafruit mini GPS, GPS over I2C, MTK3333 GPS module, tiny GPS breakout
// exact-include: Adafruit_GPS.h, Wire.h

#include <Adafruit_GPS.h>
#include <Wire.h>

Adafruit_GPS GPS(&Wire);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  GPS.begin(0x10);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
}

void loop() {
  GPS.read();
  if (GPS.newNMEAreceived()) GPS.parse(GPS.lastNMEA());

  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  if (GPS.fix) {
    Serial.print("Lat: "); Serial.print(GPS.latitudeDegrees, 6);
    Serial.print("  Lon: "); Serial.print(GPS.longitudeDegrees, 6);
    Serial.print("  Sats: "); Serial.println((int)GPS.satellites);
  } else {
    Serial.println("no fix");
  }
}
