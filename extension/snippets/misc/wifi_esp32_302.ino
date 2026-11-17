// snippet: wifi_esp32_302
// tags: wifi, httpclient
// boards: esp32, rp2040
// library: WiFi, HTTPClient
// description: Perform an HTTP GET to http://192
// prompts: perform, //192, 50/data, seconds, millis(), httpclient
// verified: compile-only
// source: kd-generated (id 302)
// exact-include: WiFi.h, HTTPClient.h

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "GreaseNet";
const char* password = "pass1234";
unsigned long prevMs = 0;

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
  if (now - prevMs >= 10000) {
    prevMs = now;
    HTTPClient http;
    http.begin("http://192.168.1.50/data");
    int code = http.GET();
    if (code > 0) {
      Serial.println(http.getString());
    } else {
      Serial.println("HTTP GET failed, error: " + String(http.errorToString(code)));
    }
    http.end();
  }
}
