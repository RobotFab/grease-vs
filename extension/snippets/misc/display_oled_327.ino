// snippet: display_oled_327
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Animate a 12-frame clockwise spinning line (like a loading indicator) on the SSD1306 128×64 OLED …
// prompts: animate, 12-frame, clockwise, spinning, (like, loading
// verified: compile-only
// source: kd-generated (id 327)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int NUM_FRAMES = 12;
const unsigned long FRAME_DURATION = 80; // Duration of each frame in milliseconds
unsigned long prevMs = 0;
int currentFrame = 0;

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= FRAME_DURATION) {
    prevMs = now;
    currentFrame = (currentFrame + 1) % NUM_FRAMES;
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    drawFrame(currentFrame);
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}

void drawFrame(int frame) {
  int angle = frame * (360 / NUM_FRAMES);
  int x = SCREEN_WIDTH / 2 + cos(radians(angle)) * (SCREEN_WIDTH / 2 - 10);
  int y = SCREEN_HEIGHT / 2 + sin(radians(angle)) * (SCREEN_HEIGHT / 2 - 10);
  display.drawLine(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, x, y, WHITE);
}
