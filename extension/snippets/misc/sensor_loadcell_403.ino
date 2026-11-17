// snippet: sensor_loadcell_403
// tags: adafruit_gfx, adafruit_ssd1306, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306, HX711
// description: Measure fill level: tare an empty container, then map HX711 readings (DOUT=3, CLK=4) to 0–100% fill
// prompts: measure, level, empty, container, hx711, readings
// verified: compile-only
// source: kd-generated (id 403)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, HX711.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DOUT_PIN 3
#define CLK_PIN  4

HX711 scale;
unsigned long prevMs = 0;
float maxWeight = 300.0; // Maximum weight in grams (full container)

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Tare empty container");
  display.display();  // SSD1306: push the framebuffer to the screen

  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();              // zero the empty container
  Serial.println("Tare complete");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  // scale.get_units() already accounts for tare; use float math (not map())
  float currentWeight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
  float fillLevel = (currentWeight / maxWeight) * 100.0f;
  fillLevel = constrain(fillLevel, 0.0f, 100.0f);

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Fill Level: ");
  display.print(fillLevel, 1);
  display.println("%");

  // Bar width proportional to fill level
  int barWidth = (int)(fillLevel / 100.0f * SCREEN_WIDTH);
  display.drawRect(0, 10, barWidth, 20, SSD1306_WHITE);
  display.display();  // SSD1306: push the framebuffer to the screen
}
