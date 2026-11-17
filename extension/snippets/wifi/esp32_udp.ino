// snippet: esp32_udp
// tags: udp, esp32, wifi, datagram, fast, telemetry, send, receive
// boards: esp32
// library: none
// description: Send sensor data as UDP datagrams from ESP32 — low latency telemetry
// prompts: ESP32 UDP telemetry, send UDP packets WiFi, low latency UDP, datagram sensor streaming ESP32
// exact-include: WiFi.h, WiFiUdp.h

#include <WiFi.h>
#include <WiFiUdp.h>

const char* SSID     = "YOUR_SSID";
const char* PASSWORD = "YOUR_PASSWORD";
const char* UDP_HOST = "192.168.1.255";  // broadcast
const int   UDP_PORT = 4210;

WiFiUDP udp;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); Serial.print("."); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  udp.begin(UDP_PORT);
}

void loop() {
  // receive
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buf[128];
    int len = udp.read(buf, sizeof(buf) - 1);
    buf[len] = '\0';
    Serial.printf("UDP RX: %s\n", buf);
  }

  // send every 100 ms
  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;
    float val = analogRead(34) * (3.3f / 4095.0f);
    char msg[32];
    snprintf(msg, sizeof(msg), "V=%.3f", val);
    udp.beginPacket(UDP_HOST, UDP_PORT);
    udp.print(msg);
    udp.endPacket();
  }
}
