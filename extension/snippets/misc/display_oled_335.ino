// snippet: display_oled_335
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Display motor speed as RPM (computed from encoder pulses on pin 2 interrupt) and direction (CW/CC…
// prompts: display, motor, speed, (computed, encoder, pulses
// verified: compile-only
// source: kd-generated (id 335)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ENC_A = 2;
volatile long position = 0;
long lastPosition = 0;
unsigned long prevMs = 0;
unsigned long lastPosTime = 0;  // Track when we last read position
const unsigned long UPDATE_INTERVAL = 250; // 4 Hz update rate
int direction = 0;

void onA() {
  position += (digitalRead(ENC_A) == LOW) ? 1 : -1;
}

void setup() {
  Serial.begin(9600);
  pinMode(ENC_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onA, CHANGE);

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

    noInterrupts();
    long pos = position;
    long posDelta = pos - lastPosition;  // change since last update
    lastPosition = pos;
    interrupts();

    // Calculate RPM from position change since last update
    // RPM = (pulses / pulses_per_rev) * 60 / time_in_seconds
    float timeSec = (now - lastPosTime) / 1000.0f;  // elapsed seconds since last update
    lastPosTime = now;
    float rpm = (posDelta * 60.0f / 20.0f) / timeSec;  // 20 pulses per revolution
    direction = (posDelta > 0) ? 1 : -1;

    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setCursor(0, 0);
    display.print("RPM: ");
    display.print(rpm, 1);
    display.setCursor(0, 10);
    display.print("Dir: ");
    display.print(direction == 1 ? "CW" : "CCW");
    display.setCursor(0, 20);
    display.print("Pos: ");
    display.print(pos);

    if (direction == 1) {
      display.drawLine(0, 30, 10, 40, SSD1306_WHITE);
      display.drawLine(10, 40, 20, 30, SSD1306_WHITE);
    } else {
      display.drawLine(0, 30, 10, 20, SSD1306_WHITE);
      display.drawLine(10, 20, 20, 30, SSD1306_WHITE);
    }

    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
