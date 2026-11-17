// snippet: esp32_websocket_server
// tags: esp32, websocket, server, broadcast, wifi, realtime, streaming, push
// boards: esp32
// library: WiFi (built-in ESP32), WebSockets
// description: WebSocket SERVER on ESP32 broadcasting a sensor reading to all connected clients
// prompts: ESP32 WebSocket server, broadcast sensor WebSocket, realtime push ESP32, WebSocketsServer ESP32, stream readings to browser
// exact-include: WiFi.h, WebSocketsServer.h

#include <WiFi.h>
#include <WebSocketsServer.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";
const int SENSOR_PIN = 34;
const unsigned long BROADCAST_MS = 200;

WebSocketsServer ws(81);
unsigned long prevMs = 0;

void onWsEvent(uint8_t client, WStype_t type, uint8_t* payload, size_t len) {
  if (type == WStype_CONNECTED) {
    Serial.print("client ");
    Serial.print(client);
    Serial.println(" connected");
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
  Serial.print("\nWebSocket server at ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81");

  ws.begin();
  ws.onEvent(onWsEvent);
}

void loop() {
  ws.loop();

  unsigned long now = millis();
  if (now - prevMs < BROADCAST_MS) return;
  prevMs = now;

  char msg[32];
  snprintf(msg, sizeof(msg), "{\"a34\":%d}", analogRead(SENSOR_PIN));
  ws.broadcastTXT(msg);
}
