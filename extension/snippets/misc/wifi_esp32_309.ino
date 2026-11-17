// snippet: wifi_esp32_309
// tags: wifi, wificlient, webserver, espmdns
// boards: esp32
// library: WiFi, WiFiClient, WebServer, ESPmDNS
// description: Advertise the device as "grease
// prompts: advertise, device, "grease, local", (espmdns, library)
// verified: compile-only
// source: kd-generated (id 309)
// exact-include: WiFi.h, WiFiClient.h, WebServer.h, ESPmDNS.h

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";
WebServer server(80);

void handlePing() {
  server.send(200, "text/plain", "pong");
}

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
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  if (MDNS.begin("grease")) {
    Serial.println("mDNS responder started");
  }

  server.on("/ping", handlePing);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
