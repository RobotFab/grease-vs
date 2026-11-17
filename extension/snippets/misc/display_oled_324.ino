// snippet: display_oled_324
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Read battery voltage from A0 (10-bit, 5 V ref, voltage divider ÷2) and display a filled progress …
// prompts: battery, voltage, (10-bit, voltage, divider, display
// verified: compile-only
// source: kd-generated (id 324)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define BATTERY_PIN A0
#define BATTERY_MIN 6000
#define BATTERY_MAX 8400

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long prevMs = 0;

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  int raw = analogRead(BATTERY_PIN);
  int voltage = map(raw, 0, 1023, BATTERY_MIN, BATTERY_MAX);
  int barWidth = map(voltage, BATTERY_MIN, BATTERY_MAX, 0, SCREEN_WIDTH);

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.fillRect(0, 0, barWidth, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();  // SSD1306: push the framebuffer to the screen
}
