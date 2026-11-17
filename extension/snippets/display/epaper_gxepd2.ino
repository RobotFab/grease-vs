// snippet: epaper_gxepd2
// tags: display, epaper, e-ink, eink, gdew, waveshare, gxepd2, spi, low-power
// boards: TBD
// library: GxEPD2, Adafruit GFX
// description: Display text on e-paper (e-ink) using GxEPD2 — low power, 2.9" or 1.54" panels
// prompts: e-paper display, e-ink screen, GxEPD2, Waveshare e-paper, low power display, e-ink text
// exact-include: GxEPD2_BW.h, Fonts/FreeMonoBold9pt7b.h

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// 2.9" Waveshare panel: 296x128 — change class to match your panel
GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(/*CS*/10, /*DC*/9, /*RST*/8, /*BUSY*/7));

void setup() {
  Serial.begin(9600);
  display.init(9600);
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(8, 20);
    display.println("Hello e-paper!");
  } while (display.nextPage());

  display.hibernate();  // low-power sleep
}

void loop() {}
