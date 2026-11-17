// snippet: wifi_esp32_303
// tags: wifi
// boards: esp32
// library: WiFi
// description: Scan for nearby WiFi networks and print each SSID with its RSSI to Serial
// prompts: nearby, networks, re-scan, seconds, millis()
// verified: compile-only
// source: kd-generated (id 303)
// exact-include: WiFi.h

#include <WiFi.h>

unsigned long prevMs = 0;
const unsigned long INTERVAL = 30000;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    scanWiFi();
  }
}

void scanWiFi() {
  Serial.println("Scanning available networks...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print("Number of networks found: ");
    Serial.println(n);
    for (int i = 0; i < n; ++i) {
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID(i));
      Serial.print("Signal strength (RSSI): ");
      Serial.println(WiFi.RSSI(i));
      Serial.println();
    }
  }
  WiFi.scanDelete();
}
