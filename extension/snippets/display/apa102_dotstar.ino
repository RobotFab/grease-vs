// snippet: apa102_dotstar
// tags: led, apa102, dotstar, spi, rgb, strip, color, adafruit, addressable
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit DotStar
// description: Drive APA102/DotStar RGB LED strip with Adafruit DotStar library
// prompts: APA102 LED strip, DotStar LEDs, addressable LED SPI, RGB LED strip APA102, DotStar color
// exact-include: Adafruit_DotStar.h, SPI.h

#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUM_LEDS   30
#define DATA_PIN    11
#define CLOCK_PIN   13

Adafruit_DotStar strip(NUM_LEDS, DATA_PIN, CLOCK_PIN, DOTSTAR_BRG);
uint32_t colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF};
int head = 0;

void setup() {
  strip.begin();
  strip.setBrightness(50);
  strip.show();  // NeoPixel: push the buffer to the LEDs
}

void loop() {
  strip.clear();
  strip.setPixelColor(head, colors[head % 5]);
  strip.show();  // NeoPixel: push the buffer to the LEDs
  head = (head + 1) % NUM_LEDS;
  delay(50);
}
