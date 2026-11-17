// snippet: romi32u4_battery
// tags: romi, pololu, romi32u4, battery, voltage, millivolts, power
// boards: romi32u4
// library: Romi32U4
// description: Read the Romi 32U4 battery voltage with readBatteryMillivolts(), millis-based.
//   readBatteryMillivolts() is a FREE FUNCTION returning uint16_t millivolts.
//   NOT: Romi32U4::getBatteryVoltage_V() — no such function or namespace.
//   NOT: analogRead(BATTERY_PIN) — use readBatteryMillivolts().
// prompts: Romi battery voltage, Romi32U4 readBatteryMillivolts, print battery millivolts Romi, Romi battery level, how much battery Romi, check the battery Romi32U4
// exact-include: Romi32U4.h

#include <Romi32U4.h>

// readBatteryMillivolts() — free function, returns uint16_t millivolts.

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 5000) return;
  prevMs = now;

  uint16_t mv = readBatteryMillivolts();
  Serial.print("Battery: ");
  Serial.print(mv);
  Serial.println(" mV");
}
