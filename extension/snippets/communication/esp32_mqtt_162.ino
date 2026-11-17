// snippet: esp32_mqtt_162
// tags: esp32, mqtt, pubsub, iot, wifi, pubsubclient
// boards: esp32
// library: WiFi, PubSubClient
// description: ESP32: read analog pin 34 every 100ms, publish to MQTT topic sensor/raw via PubSubClient
// prompts: esp32, analog, 100ms, publish, topic, sensor/raw
// verified: compile-only
// source: kd-generated (id 162)
// exact-include: WiFi.h, PubSubClient.h

#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifi;
PubSubClient mqttClient(wifi);

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "192.168.1.100";
const char* topic = "sensor/raw";

float sensorVal = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("\nWiFi connected");

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.connect("ESP32Client");
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;
    sensorVal = analogRead(34);
    mqttClient.publish(topic, String(sensorVal).c_str());
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      delay(5000);
    }
  }
}
