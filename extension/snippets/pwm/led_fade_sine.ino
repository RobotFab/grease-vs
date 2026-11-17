// snippet: led_fade_sine
// tags: pwm, fade, sine, smooth, led, analogwrite, math, pulse, glow, lamp
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Smooth LED breathing using sine wave — looks natural compared to linear fade
// prompts: LED breathing effect, sine wave LED fade, smooth LED pulse, organic breathing LED, smooth PWM fade sine, make the light pulse slowly, lamp glow softly, light do a slow pulse
// exact-include: math.h

#include <math.h>

const int LED = 9;
float angle = 0.0;

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  int val = (int)(127.5 * (1.0 + sin(angle)));
  analogWrite(LED, val);
  angle += 0.05;
  if (angle > TWO_PI) angle -= TWO_PI;
  delay(15);
}
