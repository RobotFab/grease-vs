// snippet: display_oled_340
// tags: adafruit_gfx, adafruit_ssd1306
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Display a PID debug screen on the SSD1306 128×64 OLED with four rows: setpoint, measured, error, …
// prompts: display, debug, screen, ssd1306, 128×64, setpoint
// verified: compile-only
// source: kd-generated (id 340)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
const float Kp = 2.0;
const float Ki = 0.1;
const float Kd = 0.05;
float setpoint = 0.0;
float measured = 0.0;
float error = 0.0;
float output = 0.0;
float prevError = 0.0;
float integral = 0.0;
unsigned long prevMs = 0;
const unsigned long UPDATE_INTERVAL = 100; // 10 Hz

// ADC reference voltage depends on board:
// - AVR/megaavr: 5V reference (default)
// - nano33iot/rp2040: 3.3V reference
// This sketch uses a fixed 5V reference which will overstate voltages on 3.3V boards
// To fix: use analogReference() or detect board type and scale accordingly

void setup() {
  Serial.begin(9600);
  
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
  display.println("PID Debug");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= UPDATE_INTERVAL) {
    prevMs = now;

    // Read voltage - use analogReadMilliVolts() for accurate readings across boards
    // This returns millivolts directly, avoiding reference voltage assumptions
    #if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_SAMD_NANO_33_IOT)
      // On newer cores, analogReadMilliVolts() gives millivolts directly
      setpoint = analogReadMilliVolts(A0) / 1000.0f;
      measured = analogReadMilliVolts(A1) / 1000.0f;
    #else
      // On older cores (AVR), use analogRead() with 5V reference assumption
      // Note: this will be incorrect on 3.3V boards like nano33iot/rp2040
      setpoint = analogRead(A0) * (5.0 / 1023.0);
      measured = analogRead(A1) * (5.0 / 1023.0);
    #endif

    error = setpoint - measured;
    integral += error;
    float derivative = error - prevError;
    output = Kp * error + Ki * integral + Kd * derivative;
    prevError = error;

    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("PID Debug");
    display.setCursor(0, 10);
    display.print("Setpoint: ");
    display.println(setpoint, 2);
    display.setCursor(0, 20);
    display.print("Measured: ");
    display.println(measured, 2);
    display.setCursor(0, 30);
    display.print("Error: ");
    display.println(error, 2);
    display.setCursor(0, 40);
    display.print("Output: ");
    display.println(output, 2);
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}
