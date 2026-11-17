// snippet: wifi_esp32_306
// tags: wifi, httpclient, arduinojson
// boards: esp32
// library: WiFi, HTTPClient, ArduinoJson
// description: Send an HTTP POST every 15 seconds to http://192.168.1.50/log containing timestamp and sensor value
// prompts: seconds, //192, 50/log, containing, millis(), timestamp
// verified: compile-only
// source: kd-generated (id 306)
// exact-include: WiFi.h, HTTPClient.h, ArduinoJson.h

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
unsigned long lastPostTime = 0;
const unsigned long POST_INTERVAL = 15000; // 15 seconds

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("\nWiFi connected");
}

void loop() {
  unsigned long now = millis();

  if (now - lastPostTime >= POST_INTERVAL) {
    lastPostTime = now;

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin("http://192.168.1.50/log");
      http.addHeader("Content-Type", "application/json");

      // ArduinoJson 7: JsonDocument replaces deprecated DynamicJsonDocument
      JsonDocument doc;
      doc["timestamp"] = now;
      doc["value"] = analogRead(34);
      String jsonPayload;
      serializeJson(doc, jsonPayload);

      int code = http.POST(jsonPayload);
      if (code > 0) {
        Serial.println("HTTP POST Response Code: " + String(code));
        Serial.println(http.getString());
      } else {
        Serial.println("HTTP POST failed, error: " + String(http.errorToString(code)));
      }
      http.end();
    }
  }
}
