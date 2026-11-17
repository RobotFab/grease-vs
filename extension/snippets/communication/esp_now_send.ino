// snippet: esp_now_send
// tags: esp-now, espnow, esp32, wireless, peer, mac, broadcast, fast, wifi
// boards: esp32
// library: none
// description: Send data to a peer ESP32 over ESP-NOW — replace peerMac with receiver's MAC
// prompts: ESP-NOW send, ESP32 wireless send, ESP-NOW transmit, send to ESP32 peer, ESP-NOW communication
// exact-include: esp_now.h, WiFi.h

#include <esp_now.h>
#include <WiFi.h>

uint8_t peerMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // replace with receiver MAC

typedef struct {
  float temperature;
  int   count;
} Packet;

void onSent(const uint8_t* mac, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

unsigned long prevMs = 0;
int count = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init failed"); while (1); }
  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, peerMac, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  Packet pkt = {25.5f, count++};
  esp_now_send(peerMac, (uint8_t*)&pkt, sizeof(pkt));
}
