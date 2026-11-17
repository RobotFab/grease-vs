// snippet: esp32_wifi_reconnect
// tags: wifi, esp32, reconnect, robust, watchdog, connection, retry, millis
// boards: esp32
// library: WiFi (ESP32)
// description: Robust WiFi connection with non-blocking auto-reconnect loop on ESP32.
//   Checks connection status every 30 s via millis(); reconnects without blocking loop().
// prompts: ESP32 WiFi reconnect, auto reconnect WiFi ESP32, robust WiFi ESP32, WiFi watchdog ESP32, handle WiFi disconnect ESP32
// exact-include: WiFi.h

#include <WiFi.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";

unsigned long prevWifiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 30000;

void ensureWifi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.println("WiFi lost — reconnecting...");
  WiFi.disconnect();
  WiFi.begin(SSID, PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) delay(200);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Reconnected: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("Reconnect failed — will retry");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.print("Connected: "); Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long now = millis();
  if (now - prevWifiCheck >= WIFI_CHECK_INTERVAL) {
    prevWifiCheck = now;
    ensureWifi();
  }
  // application code runs here
}
