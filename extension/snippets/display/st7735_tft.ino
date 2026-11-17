// snippet: st7735_tft
// tags: display, tft, st7735, spi, color, adafruit, 1.8inch, 128x160
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit ST7735 and ST7789, Adafruit GFX
// description: Draw on 1.8" ST7735 TFT (128x160) using Adafruit GFX library
// prompts: ST7735 TFT, 1.8 inch TFT, 128x160 display, color TFT small, Adafruit ST7735
// exact-include: Adafruit_GFX.h, Adafruit_ST7735.h, SPI.h

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS  10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
unsigned long prevMs = 0;
int counter = 0;

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  tft.setCursor(0, 0);
  tft.print("count: "); tft.println(counter++);
}
