// snippet: esp32_webserver_sta
// tags: esp32, webserver, wifi, http, station, json, status, route, rest
// boards: esp32
// library: WiFi (built-in ESP32), WebServer (built-in ESP32)
// description: ESP32 web server on the local network (station mode) serving a JSON status route
// prompts: ESP32 web server, serve JSON ESP32, HTTP status route ESP32, WebServer.h ESP32, ESP32 REST endpoint, web page on local network ESP32
// exact-include: WiFi.h, WebServer.h

#include <WiFi.h>
#include <WebServer.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";

WebServer server(80);

void handleStatus() {
  char body[96];
  snprintf(body, sizeof(body), "{\"uptime_ms\":%lu,\"free_heap\":%u}",
           (unsigned long)millis(), (unsigned int)ESP.getFreeHeap());
  server.send(200, "application/json", body);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.print("\nServer at http://");
  Serial.println(WiFi.localIP());

  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
}
