// snippet: display_oled_332
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Keep a circular buffer of 128 analogRead(A0) samples taken every 50 ms using millis()
// prompts: circular, buffer, analogread(a0), samples, taken, millis()
// verified: compile-only
// source: kd-generated (id 332)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUF_SIZE 128
int buffer[BUF_SIZE];
int bufferIndex = 0;

unsigned long prevMs = 0;
const unsigned long SAMPLE_INTERVAL = 50; // 50 ms

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= SAMPLE_INTERVAL) {
    prevMs = now;
    buffer[bufferIndex] = analogRead(A0);
    bufferIndex = (bufferIndex + 1) % BUF_SIZE;
  }

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    int index = (bufferIndex - i + BUF_SIZE) % BUF_SIZE;
    int value = buffer[index];
    int y = map(value, 0, 1023, 0, SCREEN_HEIGHT);
    display.drawLine(i, SCREEN_HEIGHT - 1, i, SCREEN_HEIGHT - y, SSD1306_WHITE);
  }
  display.display();  // SSD1306: push the framebuffer to the screen
}
