// snippet: wifinina_mqtt
// tags: wifi, wifinina, nano33iot, mqtt, pubsub, pubsubclient, broker, publish, subscribe, iot
// boards: nano33iot
// library: PubSubClient, WiFiNINA
// description: MQTT publish and subscribe on Nano 33 IoT / MKR WiFi 1010 using PubSubClient.
//   Use #include <WiFiNINA.h> — NOT #include <WiFi.h>. WiFi.h is the ESP32 header and will
//   fail to link on SAMD boards. WiFiClient must be declared after #include <WiFiNINA.h>.
// prompts: Nano 33 IoT MQTT, WiFiNINA PubSubClient, MQTT publish subscribe Nano IoT, MKR WiFi 1010 MQTT, MQTT over WiFiNINA, PubSubClient WiFiNINA, IoT MQTT Nano
// exact-include: WiFiNINA.h, PubSubClient.h

#include <WiFiNINA.h>   // NOT WiFi.h — that is the ESP32 header; it link-fails on SAMD
#include <PubSubClient.h>

const char SSID[]   = "YOUR_SSID";
const char PASS[]   = "YOUR_PASSWORD";
const char BROKER[] = "192.168.1.100";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
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
  while (!Serial && millis() < 3000);

  // Bounded retry — without a timeout, hangs forever if WiFi is unavailable
  // and loop()/MQTT never runs.
  int status = WL_IDLE_STATUS;
  unsigned long wifiStart = millis();
  while (status != WL_CONNECTED && millis() - wifiStart < 15000) {
    Serial.print("Connecting to WiFi... ");
    status = WiFi.begin(SSID, PASS);
    delay(5000);
  }
  if (status != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    while (1);
  }
  Serial.println("WiFi connected");
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
