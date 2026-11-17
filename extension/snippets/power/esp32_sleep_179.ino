// snippet: esp32_sleep_179
// tags: esp32, deep-sleep, wake, power, onewire, dallastemperature
// boards: esp32
// library: OneWire, DallasTemperature
// description: ESP32: deep sleep 30 seconds, wake, read DS18B20 on pin 3, print temperature, sleep again
// prompts: esp32, sleep, seconds, ds18b20, temperature, sleep
// verified: compile-only
// source: kd-generated (id 179)
// exact-include: OneWire.h, DallasTemperature.h

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 3
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
float tempC = 0.0;

RTC_DATA_ATTR int bootCount = 0;

const uint64_t SLEEP_SEC = 30;

void setup() {
  Serial.begin(115200);
  delay(100);

  bootCount++;
  Serial.print("Boot #"); Serial.println(bootCount);

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  if (cause == ESP_SLEEP_WAKEUP_TIMER) Serial.println("woke from timer");
  else                                  Serial.println("first boot");

  sensors.begin();

  if (cause == ESP_SLEEP_WAKEUP_TIMER) {
    sensors.requestTemperatures();  // Dallas: trigger a conversion on the 1-Wire bus (blocks ~750ms)
    tempC = sensors.getTempCByIndex(0);  // Dallas: Celsius for device index (0-based)
    // A missing sensor must NOT skip the sleep call below. `return` here — and
    // `while (1);`, the usual fix for a failed-init guard — both leave the ESP32
    // awake forever on a battery that was sized for microamps between wakeups. On
    // a sleep sketch the failure path still has to reach esp_deep_sleep_start();
    // report the bad reading and fall through.
    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.println("DS18B20 not found — sleeping anyway");
    } else {
      Serial.print("temp: ");
      Serial.println(tempC);
    }
  }

  Serial.println("going to sleep...");
  Serial.flush();

  esp_sleep_enable_timer_wakeup(SLEEP_SEC * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {}  // never reached after deep sleep
