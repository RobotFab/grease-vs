// snippet: esp32_wifi_http_get
// tags: wifi, esp32, http, get, httpclient, request, rest, api, wireless
// boards: esp32
// library: WiFi (built-in ESP32), HTTPClient
// description: HTTP GET request on ESP32 using built-in WiFi.h and HTTPClient
// prompts: ESP32 WiFi, HTTP GET, fetch URL ESP32, HTTPClient, connect WiFi ESP32, REST API ESP32, web request ESP32
// exact-include: WiFi.h, HTTPClient.h

#include <WiFi.h>
#include <HTTPClient.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
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
  if (now - prevMs < 5000) return;
  prevMs = now;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://api.example.com/data");
    int code = http.GET();
    if (code > 0) {
      Serial.println(http.getString());
    }
    http.end();
  }
}
