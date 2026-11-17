// snippet: display_oled_325
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Implement a 3-item menu ("Run", "Config", "Info") on the SSD1306 128×64 OLED
// prompts: implement, 3-item, ("run", "config", "info"), ssd1306
// verified: compile-only
// source: kd-generated (id 325)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int UP_BUTTON = 4;
const int DOWN_BUTTON = 5;
const int NUM_ITEMS = 3;
const unsigned long DEBOUNCE_MS = 50;

int selectedItem = 0;
unsigned long lastDebounceUp = 0;
unsigned long lastDebounceDown = 0;

void setup() {
  Serial.begin(9600);
  
  // Configure button pins with internal pull-up resistors
  // Buttons are active-low (pull to GND when pressed)
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Run");
  display.println("Config");
  display.println("Info");
  display.setCursor(0, selectedItem * 10);
  display.println(">");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();

  // Check for button presses (active-low with pull-up)
  if (digitalRead(UP_BUTTON) == LOW && now - lastDebounceUp > DEBOUNCE_MS) {
    lastDebounceUp = now;
    selectedItem = (selectedItem - 1 + NUM_ITEMS) % NUM_ITEMS;
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Run");
    display.println("Config");
    display.println("Info");
    display.setCursor(0, selectedItem * 10);
    display.println(">");
    display.display();  // SSD1306: push the framebuffer to the screen
  }

  if (digitalRead(DOWN_BUTTON) == LOW && now - lastDebounceDown > DEBOUNCE_MS) {
    lastDebounceDown = now;
    selectedItem = (selectedItem + 1) % NUM_ITEMS;
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Run");
    display.println("Config");
    display.println("Info");
    display.setCursor(0, selectedItem * 10);
    display.println(">");
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
