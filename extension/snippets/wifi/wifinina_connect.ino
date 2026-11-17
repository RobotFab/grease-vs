// snippet: wifinina_connect
// tags: wifi, wifinina, nano33iot, network, connect, ssid, wpa, wireless
// boards: nano33iot
// library: WiFiNINA
// description: Connect to WiFi on Nano 33 IoT — use WiFiNINA.h NOT WiFi.h
// prompts: WiFiNINA connect WiFi, Nano 33 IoT wireless, WiFiNINA SSID WPA2, Nano IoT WiFi connection, MKR WiFi 1010
// exact-include: WiFiNINA.h

#include <WiFiNINA.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);

  // Bounded retry — without a timeout, the device hangs forever in setup()
  // if WiFi is unavailable and never reaches loop().
  int status = WL_IDLE_STATUS;
  unsigned long wifiStart = millis();
  while (status != WL_CONNECTED && millis() - wifiStart < 15000) {
    Serial.print("Connecting to "); Serial.println(SSID);
    status = WiFi.begin(SSID, PASS);
    delay(5000);
  }
  if (status != WL_CONNECTED) {
    Serial.println("WiFi connect failed — continuing offline");
    return;
  }
  Serial.println("Connected");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void loop() {}
