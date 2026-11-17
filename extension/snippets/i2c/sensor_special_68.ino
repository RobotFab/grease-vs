// snippet: sensor_special_68
// tags: sensor, i2c, load-cell, hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: HX711 load cell with serial TARE command
// prompts: hx711, serial, command, declare, hx711, scale
// verified: compile-only
// source: kd-generated (id 68)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 4
#define CLK_PIN  5

HX711 scale;
float weight = 0.0;
unsigned long prevMs = 0;
String inputBuf = "";
bool lineReady = false;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void processCommand(const String& cmd) {
  if (cmd == "TARE") {
    scale.tare();  // HX711: zero the scale (averaged reading -> offset)
    Serial.println("TARE complete");
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { lineReady = true; break; }
    if (c != '\r') inputBuf += c;
  }

  if (lineReady) {
    inputBuf.trim();
    if (inputBuf.length() > 0) processCommand(inputBuf);
    inputBuf = "";
    lineReady = false;
  }

  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  if (scale.is_ready()) {
    weight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("weight: ");
    Serial.println(weight);
  }
}
