// snippet: esp32_preferences_nvs
// tags: esp32, preferences, nvs, flash, storage, persistent, boot-counter, non-volatile
// boards: esp32
// library: Preferences (built-in ESP32)
// description: Persist values across reboots and power cycles in ESP32 NVS flash with Preferences
// prompts: ESP32 Preferences, NVS storage ESP32, boot counter across restarts, save setting ESP32 flash, persistent variable ESP32, remember value after reboot
// exact-include: Preferences.h

#include <Preferences.h>

Preferences prefs;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  prefs.begin("grease", false);  // namespace, read-write
  uint32_t boots = prefs.getUInt("boots", 0) + 1;
  prefs.putUInt("boots", boots);
  prefs.end();  // close the namespace when done writing

  Serial.print("Boot count: ");
  Serial.println(boots);
}

void loop() {
}
