// snippet: wifi_esp32_319
// tags: wifi, wifiudp
// boards: esp32, nano33iot, rp2040
// library: WiFi, WiFiUdp
// description: Receive motor speed commands (-255 to 255) via WiFi UDP on port 4200
// prompts: receive, motor, speed, commands, (-255, apply
// verified: compile-only
// source: kd-generated (id 319)
// exact-include: WiFi.h, WiFiUdp.h

// ESP32/RP2040 use WiFi.h; nano33iot uses WiFiNINA.h
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <WiFiNINA.h>
#endif
#include <WiFiUdp.h>

const char* SSID     = "YOUR_SSID";
const char* PASSWORD = "YOUR_PASSWORD";
const char* UDP_HOST = "192.168.1.255";  // broadcast
const int   UDP_PORT = 4200;

WiFiUDP udp;
int motorSpeed = 0;
unsigned long lastPacketTime = 0;
const unsigned long TIMEOUT_MS = 500;

const int DIR_A = 12;
const int DIR_B = 13;
const int PWM_PIN = 14;

void setup() {
  Serial.begin(115200);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
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
    motorSpeed = atoi(buf);
    lastPacketTime = millis();
  }

  // apply motor speed, or stop if no recent packet
  if (millis() - lastPacketTime < TIMEOUT_MS) {
    motorA(motorSpeed);
  } else {
    motorA(0);
  }
}

void motorA(int speed) {
  // speed: -255 to 255
  if (speed > 0) {
    digitalWrite(DIR_A, HIGH); digitalWrite(DIR_B, LOW);
    analogWrite(PWM_PIN, speed);
  } else if (speed < 0) {
    digitalWrite(DIR_A, LOW); digitalWrite(DIR_B, HIGH);
    analogWrite(PWM_PIN, -speed);
  } else {
    digitalWrite(DIR_A, LOW); digitalWrite(DIR_B, LOW);
    analogWrite(PWM_PIN, 0);
  }
}
