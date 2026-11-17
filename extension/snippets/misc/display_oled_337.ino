// snippet: display_oled_337
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Draw a small 8×8 robot bitmap icon in the top-left of the SSD1306 128×64 OLED using drawBitmap()
// prompts: small, robot, bitmap, top-left, ssd1306, 128×64
// verified: compile-only
// source: kd-generated (id 337)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const unsigned char robotBitmap[] PROGMEM = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B01111110,
  B11111111
};

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds while we wait for the display to initialize
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 200) {
    prevMs = now;

    // Clear the top-left corner
    display.fillRect(0, 0, 8, 8, SSD1306_BLACK);

    // Draw the robot bitmap
    display.drawBitmap(0, 0, robotBitmap, 8, 8, SSD1306_WHITE);

    // Read analog value
    int analogValue = analogRead(A0);
    display.setCursor(0, 8);
    display.print("A0: ");
    display.print(analogValue);
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
