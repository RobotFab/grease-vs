// snippet: display_oled_336
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Display a 4-line status panel on the SSD1306 128×64 OLED: line 1 = mode name, line 2 = speed (0–1…
// prompts: display, 4-line, status, panel, ssd1306, 128×64
// verified: compile-only
// source: kd-generated (id 336)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned long UPDATE_INTERVAL = 100; // 100 ms update rate
unsigned long prevMs = 0;

// Global variables
String modeName = "Idle";
int speed = 0;
int batteryPercent = 100;
int errorCount = 0;

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= UPDATE_INTERVAL) {
    prevMs = now;

    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setCursor(0, 0);
    display.print(modeName);
    display.setCursor(0, 10);
    display.print("Speed: ");
    display.print(speed);
    display.print("%");
    display.setCursor(0, 20);
    display.print("Battery: ");
    display.print(batteryPercent);
    display.print("%");
    display.setCursor(0, 30);
    display.print("Errors: ");
    display.print(errorCount);

    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
