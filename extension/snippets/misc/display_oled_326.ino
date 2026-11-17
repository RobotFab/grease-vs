// snippet: display_oled_326
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Measure distance with an HC-SR04 (trig=8, echo=9) and show the value in cm as a large number plus…
// prompts: measure, distance, hc-sr04, (trig=8, echo=9), value
// verified: compile-only
// source: kd-generated (id 326)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int TRIG_PIN = 8;
const int ECHO_PIN = 9;
const unsigned long UPDATE_INTERVAL = 100; // 10 Hz update rate
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(9600);
  
  // Configure HC-SR04 pins
  pinMode(TRIG_PIN, OUTPUT);  // Trigger pin outputs 10us pulse
  pinMode(ECHO_PIN, INPUT);   // Echo pin reads return pulse duration
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;

    // Measure distance
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);  // returns echo duration in us; divide by 58 for cm (speed of sound)
    float distanceCm = duration * 0.034 / 2.0;

    // Display distance and bar
    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setCursor(0, 0);
    display.print(distanceCm);
    display.print(" cm");

    int barWidth = map(distanceCm, 0, 400, 0, SCREEN_WIDTH);
    display.drawRect(0, SCREEN_HEIGHT - 10, barWidth, 10, SSD1306_WHITE);

    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
