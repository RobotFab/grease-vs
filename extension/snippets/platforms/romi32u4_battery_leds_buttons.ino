// snippet: romi32u4_battery_leds_buttons
// tags: romi, pololu, romi32u4, battery, monitor, alert, indicator
// boards: romi32u4
// library: Romi32U4
// description: Combined Romi32U4 demo: battery monitoring with LED alert, toggled by buttons.
//   For single concerns use romi32u4_leds / romi32u4_buttons / romi32u4_battery instead.
//   LEDs are FREE FUNCTIONS: ledRed(bool), ledYellow(bool), ledGreen(bool) — NOT a class.
//   NOT: Romi32U4LEDs leds; leds.red(true) — that class does not exist.
//   readBatteryMillivolts() is also a free function (returns uint16_t mV).
//   Buttons: Romi32U4ButtonA / Romi32U4ButtonB / Romi32U4ButtonC are classes with
//   getSingleDebouncedPress() and isPressed() methods.
// prompts: Romi low battery alert LED, show battery level on LEDs when button pressed, Romi battery warning light, Romi start stop button with battery monitor
// exact-include: Romi32U4.h

#include <Romi32U4.h>

// LEDs: ledRed(bool), ledYellow(bool), ledGreen(bool) — free functions, NOT a class instance.
// Battery: readBatteryMillivolts() — free function, returns uint16_t millivolts.

Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;

const float LOW_BATTERY_V = 6.0f;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  ledRed(false);  // Romi on-board red LED (true=on)
  ledYellow(false);  // Romi on-board yellow LED (true=on)
  ledGreen(false);  // Romi on-board green LED (true=on)
  Serial.println("Ready");
}

void loop() {
  if (buttonA.getSingleDebouncedPress()) {
    ledGreen(true);  // Romi on-board green LED (true=on)
    Serial.println("A pressed");
  }
  if (buttonB.getSingleDebouncedPress()) {
    ledGreen(false);  // Romi on-board green LED (true=on)
    Serial.println("B pressed");
  }

  unsigned long now = millis();
  if (now - prevMs >= 1000) {
    prevMs = now;
    uint16_t mv      = readBatteryMillivolts();  // free function — no object needed
    float    voltage = mv / 1000.0f;
    ledRed(voltage < LOW_BATTERY_V);             // free function — NOT leds.red()
    Serial.print("Battery: "); Serial.print(voltage, 2); Serial.println(" V");
  }
}
