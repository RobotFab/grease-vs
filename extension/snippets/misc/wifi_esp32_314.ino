// snippet: wifi_esp32_314
// tags: adafruit_gfx, adafruit_ssd1306, wifi
// boards: esp32
// library: Adafruit_GFX, Adafruit_SSD1306, WiFi
// description: Display current WiFi SSID, IP address and RSSI on an SSD1306 128×64 OLED at I2C address 0x3C
// prompts: display, current, address, ssd1306, 128×64, address
// verified: compile-only
// source: kd-generated (id 314)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, WiFi.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long prevMs = 0;
const unsigned long REFRESH_INTERVAL = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Info");
  display.display();  // SSD1306: push the framebuffer to the screen
  WiFi.begin();
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) { Serial.println("WiFi failed"); while (1); }
  Serial.println("\nWiFi connected");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= REFRESH_INTERVAL) {
    prevMs = now;
    displayWiFiInfo();
  }
}

void displayWiFiInfo() {
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("SSID:");
  display.println(WiFi.SSID());
  display.println("IP:");
  display.println(WiFi.localIP());
  display.println("RSSI:");
  display.print(WiFi.RSSI());
  display.println(" dBm");
  display.display();  // SSD1306: push the framebuffer to the screen
}
