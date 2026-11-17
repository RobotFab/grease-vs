// snippet: wifi_esp32_317
// tags: wifi, httpclient
// boards: esp32
// library: WiFi, HTTPClient
// description: Try the primary server http://192
// prompts: primary, server, //192, 10/ping, first, fails
// verified: compile-only
// source: kd-generated (id 317)
// exact-include: WiFi.h, HTTPClient.h

#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";
const char* SERVER1 = "http://192.168.1.10/ping";
const char* SERVER2 = "http://192.168.1.11/ping";
unsigned long prevMs = 0;
const unsigned long INTERVAL = 10000;

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
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    if (pingServer(SERVER1)) {
      Serial.println("Server 1 responded");
    } else if (pingServer(SERVER2)) {
      Serial.println("Server 2 responded");
    } else {
      Serial.println("No server responded");
    }
  }
}

bool pingServer(const char* url) {
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  http.end();
  return code == 200;
}
