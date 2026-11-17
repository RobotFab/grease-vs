// snippet: neopixel_basic
// tags: neopixel, ws2812b, ws2812, led, strip, rgb, adafruit, fastled, pixel
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit NeoPixel
// description: Drive a WS2812B NeoPixel strip — rainbow cycle using Adafruit NeoPixel.
//   Use #define (not int/const int) for pixel count — global constructors require compile-time constants.
//   NOT: int NUM_LEDS = 8; Adafruit_NeoPixel strip(NUM_LEDS, ...) — variables cannot initialize globals.
//   CORRECT: #define NUM_PIXELS 8  then  Adafruit_NeoPixel strip(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
// prompts: NeoPixel LED strip, WS2812B, RGB LED strip, Adafruit NeoPixel, addressable LEDs, rainbow LED, smart pixel
// exact-include: Adafruit_NeoPixel.h

#include <Adafruit_NeoPixel.h>

#define PIN        6
#define NUM_PIXELS 12

Adafruit_NeoPixel strip(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
uint16_t hue = 0;
unsigned long prevMs = 0;

void setup() {
  strip.begin();
  strip.setBrightness(50);
  strip.show();  // NeoPixel: push the buffer to the LEDs
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  for (int i = 0; i < strip.numPixels(); i++) {
    uint16_t pixelHue = hue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  strip.show();  // NeoPixel: push the buffer to the LEDs
  hue += 256;
}
