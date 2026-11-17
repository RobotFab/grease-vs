// snippet: ili9341_tft
// tags: display, tft, ili9341, spi, color, adafruit, graphics, screen, 320x240
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit ILI9341, Adafruit GFX
// description: Draw text and shapes on 2.8" ILI9341 TFT using Adafruit GFX + ILI9341
// prompts: ILI9341 TFT, color TFT display, 320x240 display, TFT graphics, Adafruit ILI9341, draw on TFT
// exact-include: Adafruit_GFX.h, Adafruit_ILI9341.h, SPI.h

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS  10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Hello ILI9341");

  tft.fillRect(10, 50, 100, 40, ILI9341_BLUE);
  tft.drawCircle(200, 100, 40, ILI9341_RED);
}

void loop() {
  // update display content here
}
