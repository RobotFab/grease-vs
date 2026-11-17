// snippet: romi32u4_leds
// tags: romi, pololu, romi32u4, led, leds, blink, red, yellow, green
// boards: romi32u4
// library: Romi32U4
// description: Blink all three Romi 32U4 onboard LEDs (red, yellow, green) simultaneously, millis-based.
//   LEDs are FREE FUNCTIONS: ledRed(bool), ledYellow(bool), ledGreen(bool) — NOT a class.
//   NOT: Romi32U4LEDs leds; leds.red(true) — that class does not exist.
//   NOT: digitalWrite(LED_RED, ...) — there are no LED pin constants; use the free functions.
// prompts: Romi blink all LEDs, Romi32U4 red yellow green LED, blink three LEDs Romi, make the lights flash Romi, Romi LED on off, flash all the Romi lights, Romi32U4 ledRed ledYellow ledGreen
// exact-include: Romi32U4.h

#include <Romi32U4.h>

// ledRed(bool), ledYellow(bool), ledGreen(bool) — free functions, NOT a class instance.

unsigned long prevMs = 0;
bool ledsOn = false;

void setup() {
  ledRed(false);  // Romi on-board red LED (true=on)
  ledYellow(false);  // Romi on-board yellow LED (true=on)
  ledGreen(false);  // Romi on-board green LED (true=on)
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 250) return;
  prevMs = now;

  ledsOn = !ledsOn;
  ledRed(ledsOn);  // Romi on-board red LED (true=on)
  ledYellow(ledsOn);  // Romi on-board yellow LED (true=on)
  ledGreen(ledsOn);  // Romi on-board green LED (true=on)
}
