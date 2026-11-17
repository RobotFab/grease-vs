// snippet: ssd1306_sensor_display
// tags: display, oled, ssd1306, adafruit, i2c, sensor, values, update, realtime
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit SSD1306, Adafruit GFX
// description: Update SSD1306 OLED with live sensor readings every 500ms
// prompts: display sensor data OLED, live sensor readings, OLED dashboard, show values on screen, SSD1306 sensor display
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
unsigned long prevMs = 0;

void setup() {
  // Halt, and say why on Serial — a silent `while (1);` behind a dead display
  // leaves nothing at all to diagnose from.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found — check I2C address (0x3C vs 0x3D) and wiring");
    while (1);
  }
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  float val = analogRead(A0) * (5.0 / 1023.0);

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("V: ");
  display.println(val, 2);
  display.setTextSize(1);
  display.setCursor(0, 32);
  display.print("t="); display.print(now / 1000); display.print("s");
  display.display();  // SSD1306: push the framebuffer to the screen
}
