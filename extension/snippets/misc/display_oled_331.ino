// snippet: display_oled_331
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Show a startup splash "Grease v2" for 2 seconds on the SSD1306 128×64 OLED, then switch to a live…
// prompts: startup, splash, "grease, seconds, ssd1306, 128×64
// verified: compile-only
// source: kd-generated (id 331)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long splashStart = 0;
const unsigned long SPLASH_DURATION = 2000; // 2 seconds

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Grease v2");
  display.display();  // SSD1306: push the framebuffer to the screen
  splashStart = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - splashStart < SPLASH_DURATION) {
    // Splash screen is still active
  } else {
    // Switch to live display
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("A0: ");
    display.print(analogRead(A0));
    display.setCursor(0, 10);
    display.print("t=");
    display.print(now / 1000);
    display.print("s");
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
