// snippet: display_oled_321
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Print "Hello Grease!" centered on an SSD1306 128×64 OLED at I2C address 0x3C using Adafruit_SSD1306
// prompts: print, "hello, grease!", centered, ssd1306, 128×64
// verified: compile-only
// source: kd-generated (id 321)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - 6 * 8) / 2, (SCREEN_HEIGHT - 8) / 2); // Center text
  display.println("Hello Grease!");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 2000; // 2 seconds

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setCursor((SCREEN_WIDTH - 6 * 8) / 2, (SCREEN_HEIGHT - 8) / 2); // Center text
    display.println("Hello Grease!");
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
