// snippet: romi32u4_advanced_411
// tags: romi32u4
// boards: romi32u4
// library: Romi32U4
// description: Read the Romi battery voltage with readBatteryMillivolts()
// prompts: battery, voltage, readbatterymillivolts(), voltage, drops, below
// verified: compile-only
// source: kd-generated (id 411)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;

const float LOW_BATTERY_V = 5.5f;
bool running = false;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. A=start B=stop");
}

void loop() {
  if (buttonA.getSingleDebouncedPress()) {
    running = true;
    ledYellow(true);  // Romi on-board yellow LED (true=on)
    Serial.println("RUNNING");
  }
  if (buttonB.getSingleDebouncedPress()) {
    running = false;
    motors.setSpeeds(0, 0);
    ledYellow(false);  // Romi on-board yellow LED (true=on)
    Serial.println("STOPPED");
  }

  if (running) {
    motors.setSpeeds(100, 100);
  }

  unsigned long now = millis();
  if (now - prevMs >= 1000) {
    prevMs = now;
    uint16_t mv      = readBatteryMillivolts();
    float    voltage = mv / 1000.0f;
    ledRed(voltage < LOW_BATTERY_V);  // Romi on-board red LED (true=on)
    Serial.print("Battery: "); Serial.print(voltage, 2); Serial.println(" V");
  }
}
