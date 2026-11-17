// snippet: ssd1306_basic
// tags: display, oled, ssd1306, adafruit, i2c, 128x64, screen, text, graphics, progressbar, fillrect, drawrect, bar
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit SSD1306, Adafruit GFX
// description: Display text on 128x64 SSD1306 OLED via I2C using Adafruit library
//   The SSD1306 is MONOCHROME: the only color constants are SSD1306_WHITE and SSD1306_BLACK
//   (plus SSD1306_INVERSE). There are NO color names — SSD1306_GREEN / _RED / _BLUE do NOT
//   exist (those belong to the color SSD1331 OLED). Every draw/fill color is SSD1306_WHITE.
// prompts: SSD1306 OLED, OLED display, 128x64 display, display text OLED, Adafruit SSD1306, I2C OLED, show sensor value on screen, progress bar OLED, draw filled rectangle, bar graph display, battery level bar
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);  // monochrome — only SSD1306_WHITE/_BLACK exist (no color names)
  display.setCursor(0, 0);
  display.println("Hello, Grease!");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  // Progress / level bar. ALL graphics use SSD1306_WHITE (monochrome) — never a color name.
  static int pct = 0;
  pct = (pct + 5) % 105;                       // 0..100 demo sweep

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setCursor(0, 0);
  display.print("Level: "); display.print(pct); display.println("%");

  int x = 0, y = 20, w = 120, h = 12;
  display.drawRect(x, y, w, h, SSD1306_WHITE);                 // outline
  display.fillRect(x, y, (w * pct) / 100, h, SSD1306_WHITE);   // filled portion
  display.display();  // SSD1306: push the framebuffer to the screen
  delay(200);
}
