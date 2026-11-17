// snippet: as5600_magnetic_encoder
// tags: encoder, as5600, magnetic, angle, position, i2c, absolute, rotation
// boards: avr, esp32, megaavr, nano33iot, rp2040, nano33ble
// library: AS5600 (Rob Tillaart)
// description: Read absolute angle from AS5600 magnetic rotary encoder via I2C
// prompts: AS5600 magnetic encoder, absolute position encoder, contactless angle sensor, magnetic rotary position, 12-bit angle I2C
// exact-include: Wire.h, AS5600.h

#include <Wire.h>
#include <AS5600.h>

AS5600 encoder;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!encoder.isConnected()) {
    Serial.println("AS5600 not found");
    while (1);
  }
  encoder.setDirection(AS5600_CLOCK_WISE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;
  prevMs = now;

  float angle = encoder.readAngle() * AS5600_RAW_TO_DEGREES;
  Serial.print("angle: ");
  Serial.println(angle);
}
