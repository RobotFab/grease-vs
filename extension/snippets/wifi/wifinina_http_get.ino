// snippet: wifinina_http_get
// tags: wifi, wifinina, nano33iot, http, get, request, rest, api, client
// boards: nano33iot
// library: WiFiNINA
// description: HTTP GET request over WiFi on Nano 33 IoT using WiFiNINA.h
// prompts: WiFiNINA HTTP GET, Nano 33 IoT HTTP request, REST API Nano IoT, HTTP client WiFiNINA, web request Nano
// exact-include: WiFiNINA.h

#include <WiFiNINA.h>

const char SSID[]   = "YOUR_SSID";
const char PASS[]   = "YOUR_PASSWORD";
const char HOST[]   = "api.example.com";
const int  PORT     = 80;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);

  // Bounded retry with diagnostic output — without a timeout, hangs forever
  // if WiFi is unavailable and loop()/HTTP never runs.
  int status = WL_IDLE_STATUS;
  unsigned long wifiStart = millis();
  while (status != WL_CONNECTED && millis() - wifiStart < 15000) {
    Serial.print("Connecting to "); Serial.println(SSID);
    status = WiFi.begin(SSID, PASS);
    delay(5000);
  }
  if (status != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    while (1);
  }
  Serial.println("WiFi connected");

  if (client.connect(HOST, PORT)) {
    client.println("GET /data HTTP/1.1");
    client.print("Host: "); client.println(HOST);
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}
