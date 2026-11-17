// snippet: display_oled_333
// tags: adafruit_gfx, adafruit_ssd1306, wifi
// boards: esp32, nano33iot, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306, WiFi
// description: After connecting to WiFi, display the connected SSID and IP address on lines 1–2 of an SSD1306 12…
// prompts: after, connecting, display, connected, address, lines
// verified: compile-only
// source: kd-generated (id 333)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, WiFi.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi library selection depends on board:
// - ESP32: uses WiFi.h (built-in)
// - nano33iot: uses WiFiNINA.h (NINA module)
// - rp2040: may use WiFiNINA.h or WiFi101.h depending on board
#if defined(ESP32)
  #include <WiFi.h>
  #define WIFI_CLASS WiFiClass
  #define WIFI_SSID() WiFi.SSID()
  #define WIFI_LOCALIP() WiFi.localIP()
  #define WIFI_RSSI() WiFi.RSSI()
  #define WIFI_BEGIN(ssid, pass) WiFi.begin(ssid, pass)
  #define WIFI_STATUS() WiFi.status()
#elif defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_ARCH_RP2040)
  // Note: WiFiNINA.h is used for nano33iot and some rp2040 boards
  // This may need adjustment based on specific rp2040 board
  #include <WiFiNINA.h>
  #define WIFI_CLASS WiFiClass
  #define WIFI_SSID() WiFi.SSID()
  #define WIFI_LOCALIP() WiFi.localIP()
  #define WIFI_RSSI() WiFi.RSSI()
  #define WIFI_BEGIN(ssid, pass) WiFi.begin(ssid, pass)
  #define WIFI_STATUS() WiFi.status()
#else
  #error "WiFi library not configured for this board"
#endif

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

unsigned long prevRssiMs = 0;
const unsigned long RSSI_INTERVAL = 5000;

void setup() {
  Serial.begin(115200);
  WIFI_BEGIN(ssid, password);

  unsigned long wifiStart = millis();
  while (WIFI_STATUS() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  if (WIFI_STATUS() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }

  Serial.println("Connected to the WiFi network");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID());
  Serial.print("IP address: ");
  Serial.println(WIFI_LOCALIP());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();

  if (now - prevRssiMs >= RSSI_INTERVAL) {
    prevRssiMs = now;
    int rssi = WIFI_RSSI();
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setCursor(0, 0);
    display.print("SSID: ");
    display.println(WIFI_SSID());
    display.setCursor(0, 10);
    display.print("IP: ");
    display.println(WIFI_LOCALIP());
    display.setCursor(0, 20);
    display.print("RSSI: ");
    display.print(rssi);
    display.println(" dBm");
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
