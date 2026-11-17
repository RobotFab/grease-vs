// snippet: lis3dh_interrupt
// tags: imu, lis3dh, adafruit, interrupt, click, tap, motion, irq, i2c, accelerometer
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit LIS3DH, Adafruit Unified Sensor
// description: LIS3DH single/double-tap (click) interrupt via Adafruit library.
//   lis.setClick(clicks, threshold): clicks=1 single, clicks=2 double; threshold 0–127 (40 = ~1.2 g).
//   INT1 pin → Arduino pin 2 (INT0). Read lis.getClick() to get click source byte after interrupt.
//   getClick() bit0 = single click, bit1 = double click (bits are chip-latched, cleared on read).
//   For raw motion threshold interrupt instead, use writeRegister to configure INT1_THS/DURATION registers.
//   while(!Serial) is removed so board boots without USB — remove lis.begin(0x18) if SA0 is high (use 0x19).
// prompts: LIS3DH tap interrupt, LIS3DH click interrupt, LIS3DH single tap double tap, LIS3DH IRQ, Adafruit LIS3DH interrupt, accelerometer tap detection, LIS3DH motion interrupt
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, icm20948, icm42688
// exact-include: Wire.h, Adafruit_LIS3DH.h, Adafruit_Sensor.h

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const uint8_t INT_PIN = 2;
volatile bool tapped = false;

void tapISR() {
  tapped = true;
}

void setup() {
  Serial.begin(9600);
  if (!lis.begin(0x18)) { Serial.println("LIS3DH not found"); while (1); }
  lis.setRange(LIS3DH_RANGE_4_G);

  // Single-tap detection: threshold 40 = ~1.2 g at ±4 g range
  // Use setClick(2, 80) for double-tap instead
  lis.setClick(1, 40);

  // Route CLICK interrupt to INT1 (INT_CTRL = 0x38 bit 7, enabled by setClick)
  // INT1 → Arduino pin 2
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), tapISR, FALLING);
  Serial.println("LIS3DH tap ready");
}

void loop() {
  if (!tapped) return;
  tapped = false;

  uint8_t src = lis.getClick(); // reads CLICK_SRC register, clears latch
  if (src & 0x10) Serial.println("Single tap");
  if (src & 0x20) Serial.println("Double tap");
}
