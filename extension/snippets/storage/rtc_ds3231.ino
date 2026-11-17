// snippet: rtc_ds3231
// tags: rtc, ds3231, time, clock, i2c, timestamp, date
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: RTClib
// description: Read and set the DS3231 real-time clock over I2C using Adafruit RTClib.
//   Wiring: SDA → board SDA, SCL → board SCL, VCC → 3.3V–5V, GND → GND.
//   First run: uncomment rtc.adjust() to set the clock; comment it out after.
// prompts: DS3231 real time clock, RTC get time date, clock module I2C RTClib, timestamp RTC, set DS3231 clock
// exact-include: Wire.h, RTClib.h

#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!rtc.begin()) { Serial.println("DS3231 not found"); while (1); }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power — setting compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // To manually set: rtc.adjust(DateTime(2025, 6, 1, 12, 0, 0));
}

void loop() {
  DateTime now = rtc.now();
  Serial.print(now.year()); Serial.print('/');
  Serial.print(now.month()); Serial.print('/');
  Serial.print(now.day()); Serial.print(' ');
  Serial.print(now.hour()); Serial.print(':');
  if (now.minute() < 10) Serial.print('0');
  Serial.print(now.minute()); Serial.print(':');
  if (now.second() < 10) Serial.print('0');
  Serial.println(now.second());

  Serial.print("Temp: "); Serial.print(rtc.getTemperature()); Serial.println(" C");
  delay(1000);
}
