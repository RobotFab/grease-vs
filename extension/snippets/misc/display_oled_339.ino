// snippet: display_oled_339
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Blank the SSD1306 128×64 OLED after 30 seconds of inactivity (no button presses)
// prompts: blank, ssd1306, 128×64, after, seconds, inactivity
// verified: compile-only
// source: kd-generated (id 339)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
const int BUTTON_PIN = 4;
unsigned long lastActivityMs = 0;
const unsigned long INACTIVITY_TIMEOUT = 30000; // 30 seconds

void setup() {
  Serial.begin(9600);
  
  // Configure button pin with internal pull-up resistor
  // Button is active-low (pulls to GND when pressed)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Press to wake");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();
  if (digitalRead(BUTTON_PIN) == LOW) {
    lastActivityMs = now;
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Press to wake");
    display.display();  // SSD1306: push the framebuffer to the screen
  }

  if (now - lastActivityMs >= INACTIVITY_TIMEOUT) {
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SLEEP");
    display.display();  // SSD1306: push the framebuffer to the screen
    unsigned long tRel = millis();
    while (digitalRead(BUTTON_PIN) == LOW) {  // wait for button release, but don't hang
      if (millis() - tRel > 5000) break;
      delay(10);
    }
    lastActivityMs = now;
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Press to wake");
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
