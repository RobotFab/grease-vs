// snippet: esp32_active_259
// tags: esp32, active, serial, command, dht, arduinojson
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT, ArduinoJson
// description: ESP32 active sketch: full motor + sensor node — DHT22 temperature, HC-SR04 distance, motor speed
// prompts: esp32, active, sketch, motor, sensor, motor
// verified: compile-only
// source: kd-generated (id 259)
// exact-include: DHT.h, ArduinoJson.h

#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define TRIG_PIN 12
#define ECHO_PIN 14
#define MOTOR_PIN 5

DHT dht(DHTPIN, DHTTYPE);
int motorSpeed = 0;
float temperature = 0.0;
float distance = 0.0;

unsigned long prevSensorMs = 0;
unsigned long prevMotorMs = 0;
unsigned long prevReportMs = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.println("Send: MOTOR <speed>, STATUS");
}

void loop() {
  unsigned long now = millis();

  // Handle serial commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("MOTOR")) {
      int spaceIdx = cmd.indexOf(' ');
      if (spaceIdx > 0) {
        motorSpeed = cmd.substring(spaceIdx + 1).toInt();
        motorSpeed = constrain(motorSpeed, 0, 255);
        analogWrite(MOTOR_PIN, motorSpeed);
      }
    } else if (cmd == "STATUS") {
      sendStatusJSON();
    }
  }

  // DHT22 needs >= 2s between reads
  if (now - prevSensorMs >= 2000) {
    prevSensorMs = now;
    float t = dht.readTemperature();
    if (!isnan(t)) {
      temperature = t;
    }
  }

  // Distance can be read more frequently
  if (now - prevMotorMs >= 200) {
    prevMotorMs = now;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
    distance = duration / 58.0f;
  }

  // Report every 500ms
  if (now - prevReportMs >= 500) {
    prevReportMs = now;
    sendStatusJSON();
  }
}

void sendStatusJSON() {
  // ArduinoJson 7: JsonDocument replaces deprecated StaticJsonDocument
  JsonDocument doc;
  doc["speed"] = motorSpeed;
  doc["temperature"] = temperature;
  doc["distance"] = distance;
  serializeJson(doc, Serial);
  Serial.println();
}
