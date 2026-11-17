// snippet: combined_125
// tags: combined, multi-sensor, millis, adafruit_bmp280
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_BMP280
// description: BMP280 altitude logger: Declare float altitude, prevAltitude, verticalSpeed globally
// prompts: bmp280, altitude, logger, declare, float, altitude
// verified: compile-only
// source: kd-generated (id 125)
// exact-include: Wire.h, Adafruit_BMP280.h

#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;
float altitude = 0.0f;
float prevAltitude = 0.0f;
float verticalSpeed = 0.0f;
unsigned long prevSampleMs = 0;
unsigned long prevPrintMs = 0;
const unsigned long SAMPLE_INTERVAL = 100;
const unsigned long PRINT_INTERVAL = 500;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin(0x76)) { Serial.println("BMP280 not found"); while (1); }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  altitude = bmp.readAltitude(1013.25);
  prevAltitude = altitude;
}

void loop() {
  unsigned long now = millis();

  // Sample altitude every 100ms (separate timer from print)
  if (now - prevSampleMs >= SAMPLE_INTERVAL) {
    prevSampleMs = now;
    altitude = bmp.readAltitude(1013.25);
    verticalSpeed = (altitude - prevAltitude) / (SAMPLE_INTERVAL / 1000.0f);
    prevAltitude = altitude;
  }

  // Print every 500ms (separate timer)
  if (now - prevPrintMs >= PRINT_INTERVAL) {
    prevPrintMs = now;
    Serial.print("Altitude: "); Serial.print(altitude, 2); Serial.println(" m");
    Serial.print("Vertical Speed: "); Serial.print(verticalSpeed, 2); Serial.println(" m/s");
  }
}
