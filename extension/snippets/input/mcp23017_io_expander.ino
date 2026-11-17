// snippet: mcp23017_io_expander
// tags: mcp23017, io-expander, gpio, i2c, adafruit, 16-bit, output, input
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit MCP23017
// description: Expand Arduino GPIO with 16 extra I/O pins using MCP23017 over I2C
// prompts: MCP23017 GPIO expander, 16 extra digital pins, I2C IO expander, expand Arduino pins, add GPIO outputs inputs
// exact-include: Wire.h, Adafruit_MCP23X17.h

#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!mcp.begin_I2C(0x20)) { Serial.println("MCP23017 not found"); while (1); }

  // port A (pins 0-7): outputs
  for (int i = 0; i < 8; i++) mcp.pinMode(i, OUTPUT);
  // port B (pins 8-15): inputs with pull-ups (v2 API — pullUp() was removed)
  for (int i = 8; i < 16; i++) mcp.pinMode(i, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  // walk LEDs on port A
  static int led = 0;
  for (int i = 0; i < 8; i++) mcp.digitalWrite(i, i == led ? HIGH : LOW);
  led = (led + 1) % 8;

  // read port B buttons
  for (int i = 8; i < 16; i++) {
    if (!mcp.digitalRead(i)) {
      Serial.print("btn "); Serial.print(i - 8); Serial.println(" pressed");
    }
  }
}
