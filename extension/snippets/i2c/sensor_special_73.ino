// snippet: sensor_special_73
// tags: sensor, i2c, load-cell, rtclib
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: RTClib
// description: Read DS3231 RTC over I2C
// prompts: ds3231, include, rtclib, declare, rtc_ds3231, globally
// verified: compile-only
// source: kd-generated (id 73)
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
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 1000;  // 1 second

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC); Serial.print('-');
    Serial.print(now.month(), DEC); Serial.print('-');
    Serial.print(now.day(), DEC); Serial.print('T');
    Serial.print(now.hour(), DEC); Serial.print(':');
    Serial.print(now.minute(), DEC); Serial.print(':');
    Serial.print(now.second(), DEC); Serial.println('Z');
  }
}
