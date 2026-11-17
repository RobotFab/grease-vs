// snippet: sensor_loadcell_398
// tags: adafruit_gfx, adafruit_ssd1306, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306, HX711
// description: Display weight from HX711 (DOUT=3, CLK=4) on an SSD1306 128×64 OLED in large text (size 3)
// prompts: display, weight, hx711, (dout=3, clk=4), ssd1306
// verified: compile-only
// source: kd-generated (id 398)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, HX711.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define DOUT_PIN 3
#define CLK_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
HX711 scale;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);

  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 200) return;  // 5 Hz update rate
  prevMs = now;

  if (scale.is_ready()) {
    float weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)

    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    if (weight > 1000.0) {
      display.println("OVERLOAD");
    } else if (weight < 2.0) {
      display.println("---");
    } else {
      display.print(weight);
      display.println(" g");
    }
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
