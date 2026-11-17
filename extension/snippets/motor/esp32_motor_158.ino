// snippet: esp32_motor_158
// tags: esp32, motor, ledc, pwm, wifi, webserver
// boards: esp32
// library: WiFi, WebServer
// description: ESP32: control DC motor on pin 25 (PWM via ledcWrite) and pin 26 (direction)
// prompts: ESP32 DC motor LEDC, ESP32 motor speed direction, ledcWrite DC motor ESP32, ESP32 motor PWM H-bridge, ESP32 motor WiFi control, ESP32 motor WebServer
// verified: compile-only
// source: kd-generated (id 158)
// exact-include: WiFi.h, WebServer.h

#include <WiFi.h>
#include <WebServer.h>

const int PWM_PIN = 25;       // ESP32 LEDC PWM output to L298N ENA
const int DIR_PIN = 26;       // Direction output to L298N IN1
const int PWM_CHANNEL = 0;    // ESP32 has 16 LEDC channels (0-15)
const int PWM_FREQ = 5000;    // 5 kHz — typical for motor PWM
const int PWM_RESOLUTION = 8; // 8-bit → duty cycle 0-255

WebServer server(80);
int motorSpeed = 0;    // -255..255 (negative = reverse)
bool motorEnabled = true;

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";

void applyMotor() {
  if (!motorEnabled || motorSpeed == 0) {
    ledcWrite(PWM_CHANNEL, 0);
    return;
  }
  if (motorSpeed > 0) {
    digitalWrite(DIR_PIN, HIGH);
    ledcWrite(PWM_CHANNEL, motorSpeed);
  } else {
    digitalWrite(DIR_PIN, LOW);
    ledcWrite(PWM_CHANNEL, abs(motorSpeed));
  }
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP32 Motor Control</h1>";
  html += "<p>Speed: " + String(motorSpeed) + " (range -255..255)</p>";
  html += "<p>Enabled: " + String(motorEnabled ? "YES" : "NO") + "</p>";
  html += "<p><a href=\"/forward?speed=200\">Forward 200</a></p>";
  html += "<p><a href=\"/reverse?speed=200\">Reverse 200</a></p>";
  html += "<p><a href=\"/stop\">Stop</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleForward() {
  int speed = 200;
  if (server.hasArg("speed")) speed = server.arg("speed").toInt();
  motorSpeed = constrain(speed, 0, 255);
  applyMotor();
  server.send(200, "text/plain", "Forward at " + String(motorSpeed));
}

void handleReverse() {
  int speed = 200;
  if (server.hasArg("speed")) speed = server.arg("speed").toInt();
  motorSpeed = -constrain(speed, 0, 255);
  applyMotor();
  server.send(200, "text/plain", "Reverse at " + String(abs(motorSpeed)));
}

void handleStop() {
  motorSpeed = 0;
  applyMotor();
  server.send(200, "text/plain", "Stopped");
}

void setup() {
  Serial.begin(115200);

  // ESP32 LEDC: unlike AVR's analogWrite, you must configure a
  // channel (freq + resolution) then attach a pin to that channel.
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);
  pinMode(DIR_PIN, OUTPUT);

  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/reverse", handleReverse);
  server.on("/stop", handleStop);
  server.begin();
}

void loop() {
  server.handleClient();
}
