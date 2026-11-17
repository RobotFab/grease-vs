// snippet: tm1637_4digit
// tags: display, tm1637, 7segment, 4-digit, clock, number, digit, i2c-like
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: TM1637Display
// description: Show numbers and time on 4-digit TM1637 7-segment display with colon
// prompts: TM1637 display, 4-digit 7-segment, clock display, number display, TM1637 time, digit display
// exact-include: TM1637Display.h

#include <TM1637Display.h>

#define CLK_PIN 2
#define DIO_PIN 3

TM1637Display display(CLK_PIN, DIO_PIN);
unsigned long prevMs = 0;
int seconds = 0;

void setup() {
  display.setBrightness(0x0f);  // max brightness
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  seconds++;
  int mins = (seconds / 60) % 100;
  int secs = seconds % 60;
  int timeVal = mins * 100 + secs;

  display.showNumberDecEx(timeVal, 0b01000000, true);  // colon on
}
