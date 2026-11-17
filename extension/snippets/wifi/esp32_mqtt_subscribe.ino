// snippet: esp32_mqtt_subscribe
// tags: wifi, esp32, mqtt, pubsub, pubsubclient, subscribe, multi-topic, command, dispatch, iot
// boards: esp32
// library: PubSubClient, WiFi (ESP32)
// description: MQTT multi-topic subscribe with command dispatch on ESP32 using PubSubClient.
//   Subscribes to multiple topics; onMessage dispatches by topic string.
// prompts: ESP32 MQTT subscribe, MQTT multi-topic ESP32, PubSubClient subscribe callback, MQTT command dispatch, subscribe multiple topics ESP32
// exact-include: WiFi.h, PubSubClient.h

#include <WiFi.h>
#include <PubSubClient.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

const char SSID[]   = "YOUR_SSID";
const char PASS[]   = "YOUR_PASSWORD";
const char BROKER[] = "192.168.1.100";

WiFiClient espClient;
PubSubClient mqtt(espClient);

void onMessage(char* topic, byte* payload, unsigned int len) {
  char msg[len + 1];
  memcpy(msg, payload, len);
  msg[len] = '\0';

  if (strcmp(topic, "cmd/led") == 0) {
    digitalWrite(LED_BUILTIN, strcmp(msg, "on") == 0 ? HIGH : LOW);
  } else if (strcmp(topic, "cmd/speed") == 0) {
    int speed = atoi(msg);
    Serial.print("speed: "); Serial.println(speed);
  }
}

void reconnect() {
  while (!mqtt.connected()) {
    if (mqtt.connect("esp32-sub")) {
      mqtt.subscribe("cmd/led");
      mqtt.subscribe("cmd/speed");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
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
}
