// snippet: display_oled_329
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Split the SSD1306 128×64 OLED into two zones: top 32 pixels show A0 voltage, bottom 32 pixels sho…
// prompts: split, ssd1306, 128×64, zones, pixels, voltage
// verified: compile-only
// source: kd-generated (id 329)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
unsigned long prevMs = 0;

// ADC reference voltage depends on board:
// - AVR/megaavr: 5V reference (default)
// - nano33iot/rp2040: 3.3V reference
// This sketch uses a fixed 5V reference which will overstate voltages on 3.3V boards
// To fix: use analogReference() or detect board type and scale accordingly

void setup() {
  // Configure ADC reference based on board
  #if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_SAMD_NANO_33_IOT)
    // 3.3V boards - use internal 3.3V reference if available
    // Note: analogReference(DEFAULT) uses VCC which may be 3.3V or 5V depending on board
    // For accurate voltage readings, use analogReadMilliVolts() on newer cores
  #endif
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("A0 Voltage");
  display.setCursor(0, 32);
  display.println("A1 Voltage");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 200) return;  // 5 Hz refresh rate
  prevMs = now;

  // Read voltage - use analogReadMilliVolts() for accurate readings across boards
  // This returns millivolts directly, avoiding reference voltage assumptions
  #if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_SAMD_NANO_33_IOT)
    // On newer cores, analogReadMilliVolts() gives millivolts directly
    float a0Val = analogReadMilliVolts(A0) / 1000.0f;
    float a1Val = analogReadMilliVolts(A1) / 1000.0f;
  #else
    // On older cores (AVR), use analogRead() with 5V reference assumption
    // Note: this will be incorrect on 3.3V boards like nano33iot/rp2040
    float a0Val = analogRead(A0) * (5.0 / 1023.0);
    float a1Val = analogRead(A1) * (5.0 / 1023.0);
  #endif

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("A0: ");
  display.println(a0Val, 2);
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.print("A1: ");
  display.println(a1Val, 2);
  display.display();  // SSD1306: push the framebuffer to the screen
}
