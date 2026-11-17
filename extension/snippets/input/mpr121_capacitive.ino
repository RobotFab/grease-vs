// snippet: mpr121_capacitive
// tags: touch, mpr121, capacitive, adafruit, i2c, button, sensor
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit MPR121
// description: 12-key capacitive touch sensing with Adafruit MPR121 breakout
// prompts: MPR121 capacitive touch, 12-key touch sensor, capacitive buttons, touch pad sensor, Adafruit touch keyboard
// exact-include: Wire.h, Adafruit_MPR121.h

#include <Wire.h>
#include <Adafruit_MPR121.h>

Adafruit_MPR121 cap;
uint16_t lastTouched = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found");
    while (1);
  }
  Serial.println("MPR121 ready");
}

void loop() {
  uint16_t touched = cap.touched();
  for (uint8_t i = 0; i < 12; i++) {
    if ((touched & (1 << i)) && !(lastTouched & (1 << i))) {
      Serial.print("touched: "); Serial.println(i);
    }
    if (!(touched & (1 << i)) && (lastTouched & (1 << i))) {
      Serial.print("released: "); Serial.println(i);
    }
  }
  lastTouched = touched;
}
