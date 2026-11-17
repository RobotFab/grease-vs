// snippet: esp32_ota_update
// tags: wifi, esp32, ota, over-the-air, arduinoota, firmware update, wireless upload
// boards: esp32
// library: ArduinoOTA (built-in ESP32), WiFi (ESP32)
// description: OTA firmware update on ESP32 using ArduinoOTA. After upload the board
//   appears in the Arduino IDE / arduino-cli port list as a network target.
// prompts: ESP32 OTA, over the air update ESP32, ArduinoOTA ESP32, wireless firmware update, OTA upload ESP32
// exact-include: WiFi.h, ArduinoOTA.h

#include <WiFi.h>
#include <ArduinoOTA.h>

const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASSWORD";

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { delay(500); }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("esp32-ota");
  // Use a YOUR_X placeholder like every other credential in this corpus —
  // never hardcode a real OTA password in a reference snippet.
  ArduinoOTA.setPassword("YOUR_OTA_PASSWORD");

  ArduinoOTA.onStart([]()  { Serial.println("OTA start"); });
  ArduinoOTA.onEnd([]()    { Serial.println("\nOTA done"); });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
    Serial.printf("OTA %u%%\r", done * 100 / total);
  });
  ArduinoOTA.onError([](ota_error_t e) {
    Serial.printf("OTA error[%u]\n", e);
  });

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  // application code runs here alongside OTA polling
}
