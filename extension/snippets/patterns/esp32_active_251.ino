// snippet: esp32_active_251
// tags: esp32, active, serial, command, wifi, httpclient
// boards: esp32
// library: WiFi, HTTPClient
// description: ESP32 active sketch: WIFI_SEND <value> posts to http://192.168.1.100:3000/recv
// prompts: esp32, active, sketch, wifi_send, <value>, posts
// verified: compile-only
// source: kd-generated (id 251)
// exact-include: WiFi.h, HTTPClient.h

#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";
WiFiClient client;

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
  Serial.println("Send: WIFI_SEND <value>, READ_ADC, PING");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "PING") {
      Serial.println("PONG");
    } else if (cmd.startsWith("WIFI_SEND")) {
      // Parse value after "WIFI_SEND "
      int spaceIdx = cmd.indexOf(' ');
      if (spaceIdx > 0) {
        String value = cmd.substring(spaceIdx + 1);
        handleWIFISend(value);
      }
    } else if (cmd == "READ_ADC") {
      int adcValue = analogRead(34);
      Serial.print("adc: "); Serial.println(adcValue);
    }
  }
}

void handleWIFISend(String value) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }
  HTTPClient http;
  http.begin(client, "http://192.168.1.100:3000/recv");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST("value=" + value);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
