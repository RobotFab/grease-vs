// snippet: wifi_esp32_312
// tags: wifi, httpclient
// boards: esp32
// library: WiFi, HTTPClient
// description: Use deep sleep: wake every 30 seconds, connect to WiFi, POST the value of analogRead(34) to http://192.168.1.50/data
// prompts: sleep, seconds, connect, value, analogread(34), //192
// verified: compile-only
// source: kd-generated (id 312)
// exact-include: WiFi.h, HTTPClient.h

#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";
WiFiClient client;
float sensorVal = 0.0;

RTC_DATA_ATTR int bootCount = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  bootCount++;
  Serial.print("Boot #"); Serial.println(bootCount);

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  if (cause == ESP_SLEEP_WAKEUP_TIMER) Serial.println("woke from timer");
  else                                  Serial.println("first boot");

  if (cause == ESP_SLEEP_WAKEUP_TIMER) {
    sensorVal = analogRead(34) * (3.3f / 4095.0f);

    // Connect WiFi before sending — deep sleep powers down the radio
    WiFi.begin(SSID, PASS);
    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected");
      sendSensorData();
    } else {
      Serial.println("WiFi failed — skipping POST");
    }
  }

  Serial.println("going to sleep...");
  Serial.flush();

  esp_sleep_enable_timer_wakeup(30 * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {}  // never reached after deep sleep

void sendSensorData() {
  HTTPClient http;
  http.begin(client, "http://192.168.1.50/data");
  http.addHeader("Content-Type", "application/json");
  String jsonPayload = "{\"value\": " + String(sensorVal) + "}";
  int code = http.POST(jsonPayload);
  if (code > 0) {
    Serial.println("HTTP POST Response Code: " + String(code));
    Serial.println(http.getString());
  } else {
    Serial.println("HTTP POST failed, error: " + String(http.errorToString(code)));
  }
  http.end();
}
