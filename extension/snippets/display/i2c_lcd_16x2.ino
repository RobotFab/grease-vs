// snippet: i2c_lcd_16x2
// tags: display, lcd, 16x2, i2c, liquidcrystal, pcf8574, screen, text
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: LiquidCrystal I2C
// description: Print text on 16x2 LCD with I2C backpack using LiquidCrystal_I2C
// prompts: LCD display, 16x2 LCD, I2C LCD, LiquidCrystal, LCD text, display message LCD, character display
// exact-include: Wire.h, LiquidCrystal_I2C.h

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long prevMs = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Grease Ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  lcd.setCursor(0, 1);
  lcd.print("t=");
  lcd.print(now / 1000);
  lcd.print("s   ");
}
