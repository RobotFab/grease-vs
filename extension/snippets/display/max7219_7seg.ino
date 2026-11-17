// snippet: max7219_7seg
// tags: display, max7219, 7segment, led-matrix, spi, number, digit, ledcontrol
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: LedControl
// description: Display numbers on MAX7219-driven 7-segment display or 8x8 LED matrix
// prompts: MAX7219 7-segment, LED matrix display, 7-segment number, display digits, LedControl library, 8x8 LED matrix
// exact-include: LedControl.h

#include <LedControl.h>

// LedControl(DIN, CLK, CS, numDevices)
LedControl lc(12, 11, 10, 1);
unsigned long prevMs = 0;
int count = 0;

void setup() {
  lc.shutdown(0, false);   // wake up
  lc.setIntensity(0, 8);   // brightness 0-15
  lc.clearDisplay(0);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  // show count on 4 rightmost digits
  lc.setDigit(0, 3, (count / 1000) % 10, false);
  lc.setDigit(0, 2, (count / 100)  % 10, false);
  lc.setDigit(0, 1, (count / 10)   % 10, false);
  lc.setDigit(0, 0, count           % 10, false);
  count = (count + 1) % 10000;
}
