// snippet: wifi_esp32_307
// tags: wifi, time
// boards: esp32
// library: WiFi, time
// description: Sync time from NTP server pool
// prompts: server, configtime(), after, current, seconds, millis()
// verified: compile-only
// source: kd-generated (id 307)
// exact-include: WiFi.h, time.h

#include <WiFi.h>
#include <time.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

WiFiClient client;
time_t now;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("Connected to the WiFi network");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for time sync...");
  while (!time(nullptr)) {
    delay(1000);
  }
  Serial.println("Time synced");
}

void loop() {
  static unsigned long prevMs = 0;
  if (millis() - prevMs >= 60000) {
    prevMs = millis();
    time(&now);
    Serial.print("Current UTC time: ");
    Serial.println(ctime(&now));
  }
}
