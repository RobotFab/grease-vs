// snippet: esp32_wifi_167
// tags: esp32, wifi, http, webserver, httpclient
// boards: esp32
// library: WiFi, HTTPClient
// description: ESP32: connect to WiFi, HTTP POST sensor reading from pin 34 to http://192
// prompts: esp32, connect, sensor, reading, //192, 3000/data
// verified: compile-only
// source: kd-generated (id 167)
// exact-include: WiFi.h, HTTPClient.h

#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";
WiFiClient client;
float sensorVal = 0.0;

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
  if (now - prevMs >= 5000) {
    prevMs = now;
    sensorVal = analogRead(34) * (3.3f / 4095.0f);
    sendSensorData();
  }
}

void sendSensorData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, "http://192.168.1.100:3000/data");
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
}
