// snippet: esp32_mdns
// tags: wifi, esp32, mdns, zeroconf, bonjour, hostname, service discovery, http
// boards: esp32
// library: ESPmDNS (built-in ESP32), WiFi (ESP32), WebServer (ESP32)
// description: Register an mDNS hostname and HTTP service on ESP32 so the board is
//   reachable at http://esp32.local on the local network without knowing the IP.
// prompts: ESP32 mDNS, esp32.local, mDNS hostname ESP32, Bonjour ESP32, zero-config network, service discovery ESP32
// exact-include: WiFi.h, ESPmDNS.h, WebServer.h

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Hello from esp32.local");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32")) {
    Serial.println("mDNS init failed");
    while (1) delay(1000);
  }
  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS: http://esp32.local");

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
