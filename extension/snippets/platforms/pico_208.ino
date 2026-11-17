// snippet: pico_208
// tags: pico, rp2040, raspberry-pi
// boards: rp2040
// library: none
// description: RPi Pico: active sketch — responds to serial commands, reads ADC on demand
// prompts: serial command Pico, interactive sketch RP2040, read ADC on command Pi Pico, serial request response Pico, command driven Pico, active mode RP2040
// verified: compile-only
// source: kd-generated (id 208)
// exact-include: Arduino.h

#include <Arduino.h>

char cmdBuf[32];
int cmdIndex = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processCommand(cmdBuf);
      cmdIndex = 0;
    } else if (cmdIndex < 31) {
      cmdBuf[cmdIndex++] = c;
    }
  }
}

void processCommand(char* cmd) {
  if (strncmp(cmd, "PING", 4) == 0) {
    Serial.println("PONG");
  } else if (strncmp(cmd, "READ_ADC", 8) == 0) {
    int pin = cmd[9] - '0';
    int adcValue = analogRead(pin);
    Serial.print("ADC:");
    Serial.println(adcValue);
  } else if (strncmp(cmd, "SET_PIN", 7) == 0) {
    int pin = cmd[8] - '0';
    int state = cmd[10] - '0';
    // Must set pinMode before digitalWrite — ESP32/RP2040 GPIO defaults
    // to input on boot, so digitalWrite on an unconfigured pin is a no-op.
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
  }
}
