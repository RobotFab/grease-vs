// snippet: nano33iot_296
// tags: nano33iot, samd, imu, wifi, wifinina, arduino_lsm6ds3
// boards: nano33iot
// library: WiFiNINA, Arduino_LSM6DS3
// description: Arduino Nano 33 IoT: WiFi connection and sensor upload
// prompts: arduino, connection, sensor, upload, include, wifinina
// verified: compile-only
// source: kd-generated (id 296)
// exact-include: WiFiNINA.h, Arduino_LSM6DS3.h

#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
WiFiClient client;

float ax, ay, az;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);

  // WiFi connection with timeout (30 seconds)
  unsigned long connectStart = millis();
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED && millis() - connectStart < 30000) {
    status = WiFi.begin(ssid, password);
    delay(500);
  }
  if (status != WL_CONNECTED) {
    Serial.println("WiFi connection failed");
    while (1);
  }
  Serial.println("Connected to WiFi");

  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 5000) {  // every 5 seconds
    prevMs = now;

    if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
      IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()

      if (client.connect("192.168.1.100", 3000)) {
        String json = "{\"ax\":" + String(ax) + ",\"ay\":" + String(ay) + ",\"az\":" + String(az) + "}";
        client.println("POST /imu HTTP/1.1");
        client.println("Host: 192.168.1.100");
        client.println("Content-Type: application/json");
        client.println("Content-Length: " + String(json.length()));
        client.println();
        client.print(json);

        while (client.available()) {
          Serial.write(client.read());
        }
        client.stop();
      }
    }
  }
}
