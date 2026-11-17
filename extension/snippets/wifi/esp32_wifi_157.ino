// snippet: esp32_wifi_157
// tags: esp32, wifi, http, webserver, dht
// boards: esp32
// library: WiFi, WebServer, DHT
// description: ESP32: read DHT22 on pin 4
// prompts: esp32, dht22, serve, temperature, /temp, webserver
// verified: compile-only
// source: kd-generated (id 157)
// exact-include: WiFi.h, WebServer.h, DHT.h

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);
float temperature = 0.0;

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";

void handleRoot() {
  String html = "<html><body><h1>Temperature: " + String(temperature) + "°C</h1></body></html>";
  server.send(200, "text/html", html);
}

void handleTemp() {
  temperature = dht.readTemperature();
  if (isnan(temperature)) {
    server.send(500, "text/plain", "Failed to read temperature");
  } else {
    server.send(200, "text/plain", String(temperature));
  }
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
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  server.on("/", handleRoot);
  server.on("/temp", handleTemp);
  server.begin();
}

void loop() {
  server.handleClient();
}
