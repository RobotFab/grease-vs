// snippet: wifinina_web_server
// tags: wifi, wifinina, nano33iot, webserver, http, server, html, get
// boards: nano33iot
// library: WiFiNINA
// description: Tiny HTTP server on Nano 33 IoT — serves sensor data as JSON on GET /
// prompts: WiFiNINA web server, Nano 33 IoT HTTP server, serve JSON Nano IoT, local web server WiFiNINA
// exact-include: WiFiNINA.h

#include <WiFiNINA.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";

WiFiServer server(80);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); Serial.print("."); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.print("\nIP: "); Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();

  float t = 23.5;  // replace with real sensor
  String body = "{\"temperature\":" + String(t) + "}";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  client.println(body);
  client.stop();
}
