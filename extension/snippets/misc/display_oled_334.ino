// snippet: display_oled_334
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Implement a count-up stopwatch on the SSD1306 128×64 OLED showing MM:SS
// prompts: implement, count-up, stopwatch, ssd1306, 128×64, showing
// verified: compile-only
// source: kd-generated (id 334)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int START_BUTTON_PIN = 4;
const int RESET_BUTTON_PIN = 5;
const int DEBOUNCE_DELAY = 50;

bool isRunning = false;
unsigned long startTime = 0;
unsigned long lastDebounceTime = 0;

// Button state latches to prevent rapid toggling when button is held
bool lastStartButtonState = HIGH;
bool lastResetButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();

  // Read button states
  int startButtonState = digitalRead(START_BUTTON_PIN);
  int resetButtonState = digitalRead(RESET_BUTTON_PIN);

  // Start button - toggle run/stop with edge detection
  if (startButtonState == LOW && lastStartButtonState == HIGH && (now - lastDebounceTime) > DEBOUNCE_DELAY) {
    lastDebounceTime = now;
    if (isRunning) {
      isRunning = false;
    } else {
      isRunning = true;
      startTime = now;
    }
  }
  lastStartButtonState = startButtonState;

  // Reset button - reset with edge detection
  if (resetButtonState == LOW && lastResetButtonState == HIGH && (now - lastDebounceTime) > DEBOUNCE_DELAY) {
    lastDebounceTime = now;
    isRunning = false;
    startTime = 0;
  }
  lastResetButtonState = resetButtonState;

  if (isRunning) {
    unsigned long elapsedTime = now - startTime;
    int minutes = elapsedTime / 60000;
    int seconds = (elapsedTime % 60000) / 1000;
    int milliseconds = (elapsedTime % 1000);

    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setCursor(0, 0);
    display.print(minutes);
    display.print(":");
    if (seconds < 10) display.print("0");
    display.print(seconds);
    display.print(".");
    if (milliseconds < 100) display.print("0");
    if (milliseconds < 10) display.print("0");
    display.print(milliseconds);
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
