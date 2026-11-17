// snippet: esp32_mqtt
// tags: wifi, esp32, mqtt, pubsub, pubsubclient, broker, publish, subscribe, iot
// boards: esp32
// library: PubSubClient, WiFi (ESP32)
// description: MQTT publish and subscribe on ESP32 using PubSubClient
// prompts: ESP32 MQTT, PubSubClient broker, IoT MQTT publish subscribe, MQTT sensor data ESP32, MQTT over WiFi
// exact-include: WiFi.h, PubSubClient.h

#include <WiFi.h>
#include <PubSubClient.h>

const char SSID[]   = "YOUR_SSID";
const char PASS[]   = "YOUR_PASSWORD";
const char BROKER[] = "192.168.1.100";

WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long prevMs = 0;

void onMessage(char* topic, byte* payload, unsigned int len) {
  Serial.print("msg ["); Serial.print(topic); Serial.print("]: ");
  for (unsigned int i = 0; i < len; i++) Serial.print((char)payload[i]);
  Serial.println();
}

void reconnect() {
  while (!mqtt.connected()) {
    if (mqtt.connect("arduino-client")) {
      mqtt.subscribe("inbound/topic");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  mqtt.setServer(BROKER, 1883);
  mqtt.setCallback(onMessage);
}

void loop() {
  if (!mqtt.connected()) reconnect();
  mqtt.loop();

  unsigned long now = millis();
  if (now - prevMs >= 2000) {
    prevMs = now;
    mqtt.publish("outbound/topic", "hello");
  }
}
