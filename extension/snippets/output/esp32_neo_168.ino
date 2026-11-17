// snippet: esp32_neo_168
// tags: esp32, neopixel, ws2812b, rgb, addressable, adafruit_neopixel
// boards: esp32
// library: Adafruit_NeoPixel
// description: ESP32: WS2812B NeoPixel strip 8 LEDs on pin 16
// prompts: esp32, ws2812b, neopixel, strip, receive, color
// verified: compile-only
// source: kd-generated (id 168)
// exact-include: Adafruit_NeoPixel.h

#include <Adafruit_NeoPixel.h>

#define PIN 16
#define NUM_LEDS 8
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();  // Initialize all pixels to off
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.startsWith("COLOR")) {
      // Parse "COLOR <r> <g> <b>" — use sscanf to handle variable-width
      // numbers. Fixed substring offsets (6/9/12) break when any channel
      // isn't exactly 3 digits (e.g. "COLOR 255 5 0").
      int r = 0, g = 0, b = 0;
      // sscanf on the C-string after "COLOR" — skip the command word
      if (sscanf(command.c_str() + 5, " %d %d %d", &r, &g, &b) == 3) {
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);
        setColor(r, g, b);
      } else {
        Serial.println("Usage: COLOR <r> <g> <b>");
      }
    }
  }
}

void setColor(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();  // NeoPixel: push the buffer to the LEDs
}
