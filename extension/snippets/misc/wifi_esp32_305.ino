// snippet: wifi_esp32_305
// tags: wifi
// boards: esp32, nano33iot, rp2040
// library: WiFi
// description: Monitor WiFi connection status with millis()
// prompts: monitor, connection, status, millis(), disconnected, seconds
// verified: compile-only
// source: kd-generated (id 305)
// exact-include: WiFi.h

// ESP32 uses WiFi.h; nano33iot uses WiFiNINA.h
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <WiFiNINA.h>
#endif

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
}

void loop() {
  unsigned long now = millis();

  if (WiFi.status() == WL_CONNECTED) {
    if (lastReconnectAttempt > 0) {
      Serial.println("connected");
      lastReconnectAttempt = 0;
    }
  } else {
    if (now - lastReconnectAttempt >= RECONNECT_INTERVAL) {
      Serial.println("reconnecting");
      WiFi.disconnect();  // clear stale connection state before retrying
      WiFi.begin(ssid, password);
      lastReconnectAttempt = now;
    }
  }
}
