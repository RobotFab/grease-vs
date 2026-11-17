// snippet: wifi_esp32_310
// tags: wifi, wifiudp
// boards: esp32, nano33iot, rp2040
// library: WiFi, WiFiUdp
// description: Send sensor data as UDP packets to broadcast address 192.168.1.255
// prompts: sensor, packets, broadcast, address, millis(), packet
// verified: compile-only
// source: kd-generated (id 310)
// exact-include: WiFi.h, WiFiUdp.h

// ESP32/RP2040 use WiFi.h; nano33iot uses WiFiNINA.h
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <WiFiNINA.h>
#endif
#include <WiFiUdp.h>

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";
const char* UDP_HOST = "192.168.1.255";
const int UDP_PORT = 4210;
WiFiUDP udp;
unsigned long prevMs = 0;

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
  udp.begin(UDP_PORT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;
    int raw = analogRead(34);
    char msg[64];
    snprintf(msg, sizeof(msg), "id=1,val=%d,t=%lu", raw, now);
    udp.beginPacket(UDP_HOST, UDP_PORT);
    udp.print(msg);
    udp.endPacket();
  }
}
