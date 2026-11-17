// snippet: active_mode_109
// tags: active, serial, command, response, onewire, dallastemperature
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: Active sketch: GET_TEMP reads DS18B20 on pin 3, replies T:<celsius>
// prompts: active, sketch, get_temp, reads, ds18b20, replies
// verified: compile-only
// source: kd-generated (id 109)
// exact-include: OneWire.h, DallasTemperature.h

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 3
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
float tempC = 0.0;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  Serial.println("DS18B20 ready — send GET_TEMP");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "GET_TEMP") {
      // requestTemperatures() issues a 1-Wire conversion; wait for it
      sensors.requestTemperatures();  // Dallas: trigger a conversion on the 1-Wire bus (blocks ~750ms)
      tempC = sensors.getTempCByIndex(0);  // Dallas: Celsius for device index (0-based)
      if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("ERR: sensor disconnected");
      } else {
        Serial.print("T:");
        Serial.println(tempC);
      }
    }
  }
}
