// snippet: esp32_deep_sleep
// tags: esp32, deep-sleep, power, low-power, wakeup, timer, rtc, battery
// boards: esp32
// library: none
// description: Put ESP32 into deep sleep and wake on timer — reduces power from 240mA to ~10µA
// prompts: ESP32 deep sleep, low power ESP32, sleep wake timer, battery powered ESP32, RTC wakeup, power saving ESP32
// exact-include: esp_sleep.h

#include <esp_sleep.h>

RTC_DATA_ATTR int bootCount = 0;  // persists across deep sleep

const uint64_t SLEEP_SEC = 30;

void setup() {
  Serial.begin(115200);
  delay(100);

  bootCount++;
  Serial.print("Boot #"); Serial.println(bootCount);

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  if (cause == ESP_SLEEP_WAKEUP_TIMER) Serial.println("woke from timer");
  else                                  Serial.println("first boot");

  // do work here
  float t = 25.0f;  // read sensor
  Serial.print("temp: "); Serial.println(t);

  Serial.println("going to sleep...");
  Serial.flush();

  esp_sleep_enable_timer_wakeup(SLEEP_SEC * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {}  // never reached after deep sleep
