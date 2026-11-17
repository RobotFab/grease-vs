// snippet: esp32_websocket_client
// tags: websocket, esp32, wifi, ws, socket, real-time, arduinojson, asyncwebsocket
// boards: esp32
// library: WebSockets
// description: ESP32 WebSocket client — connect to a WS server and send/receive messages
// prompts: ESP32 WebSocket client, real-time WebSocket connection, WS protocol ESP32, WebSockets library ESP32
// exact-include: WiFi.h, WebSocketsClient.h

#include <WiFi.h>
#include <WebSocketsClient.h>

const char* SSID     = "YOUR_SSID";
const char* PASSWORD = "YOUR_PASSWORD";
const char* WS_HOST  = "192.168.1.100";
const int   WS_PORT  = 8080;

WebSocketsClient ws;
unsigned long prevMs = 0;

void onEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.printf("WS RX: %s\n", payload);
  } else if (type == WStype_CONNECTED) {
    Serial.println("WS connected");
    ws.sendTXT("hello from ESP32");
  } else if (type == WStype_DISCONNECTED) {
    Serial.println("WS disconnected");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); Serial.print("."); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  ws.begin(WS_HOST, WS_PORT, "/");
  ws.onEvent(onEvent);
  ws.setReconnectInterval(5000);
}

void loop() {
  ws.loop();
  unsigned long now = millis();
  if (now - prevMs >= 2000) {
    prevMs = now;
    ws.sendTXT("ping");
  }
}
