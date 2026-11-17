// snippet: sensor_temp_48
// tags: temperature, sensor, millis, onewire, dallastemperature
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: On serial command TEMP, read DS18B20 on pin 3 and reply T:<celsius>
// prompts: serial, command, ds18b20, <celsius>, declare, dallastemperature
// verified: compile-only
// source: kd-generated (id 48)
// exact-include: OneWire.h, DallasTemperature.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 3
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
float tempC = 0.0;
bool readingTemp = false;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "TEMP") {
    readingTemp = true;
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }

  if (readingTemp) {
    unsigned long now = millis();
    if (now - prevMs >= 1000) {
      prevMs = now;
      sensors.requestTemperatures();  // Dallas: trigger a conversion on the 1-Wire bus (blocks ~750ms)
      tempC = sensors.getTempCByIndex(0);  // Dallas: Celsius for device index (0-based)
      if (tempC != DEVICE_DISCONNECTED_C) {
        Serial.print("T:");
        Serial.println(tempC);
        readingTemp = false;
      }
    }
  }
}
