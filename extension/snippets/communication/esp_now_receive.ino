// snippet: esp_now_receive
// tags: esp-now, espnow, esp32, wireless, receive, peer, callback, fast
// boards: esp32
// library: none
// description: Receive ESP-NOW packets — print sender MAC and payload (pair with esp_now_send)
// prompts: ESP-NOW receive, ESP32 wireless receive, ESP-NOW peer communication, receive from ESP32, ESP-NOW listener
// exact-include: esp_now.h, WiFi.h

#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  float temperature;
  int   count;
} Packet;

void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  Packet pkt;
  memcpy(&pkt, data, sizeof(pkt));
  Serial.print("From: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X%s", info->src_addr[i], i < 5 ? ":" : "  ");
  }
  Serial.print("T="); Serial.print(pkt.temperature);
  Serial.print(" cnt="); Serial.println(pkt.count);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("MAC: "); Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init failed"); while (1); }
  esp_now_register_recv_cb(onReceive);
}

void loop() {}
