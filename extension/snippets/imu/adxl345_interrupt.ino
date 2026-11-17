// snippet: adxl345_interrupt
// tags: imu, adxl345, adafruit, interrupt, freefall, activity, irq, i2c, accelerometer
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit ADXL345, Adafruit Unified Sensor
// description: ADXL345 activity and free-fall interrupts via Adafruit library.
//   INT1 pin → Arduino pin 2 (INT0). Uses writeRegister() for interrupt config registers.
//   ADXL345_REG_THRESH_ACT  (0x24): activity threshold, 62.5 mg/LSB
//   ADXL345_REG_THRESH_FF   (0x28): free-fall threshold, 62.5 mg/LSB (typ: 0x07–0x09)
//   ADXL345_REG_TIME_FF     (0x29): free-fall time, 5 ms/LSB (typ: 0x14–0x46 = 100–350 ms)
//   ADXL345_REG_INT_ENABLE  (0x2E): bit 4 = activity, bit 2 = free-fall
//   ADXL345_REG_INT_SOURCE  (0x30): read to clear interrupt latch
//   Adafruit Unified Sensor lib does NOT expose high-level interrupt methods — use writeRegister directly.
// prompts: ADXL345 interrupt, ADXL345 free fall interrupt, ADXL345 activity interrupt, ADXL345 IRQ, accelerometer fall detection ADXL, ADXL345 motion detection
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, bno055, bno085, icm20948, icm42688, lis3dh
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_ADXL345_U.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel(12345);

const uint8_t INT_PIN = 2;

volatile bool intFired = false;

void accelISR() {
  intFired = true;
}

void setup() {
  Serial.begin(9600);
  if (!accel.begin()) { Serial.println("ADXL345 not found"); while (1); }
  accel.setRange(ADXL345_RANGE_4_G);

  // Free-fall: threshold ~437 mg, time ~100 ms (tune for your application)
  accel.writeRegister(0x28, 0x07); // THRESH_FF: 7 × 62.5 mg = 437 mg
  accel.writeRegister(0x29, 0x14); // TIME_FF:   20 × 5 ms  = 100 ms

  // Activity: threshold ~1 g
  accel.writeRegister(0x24, 0x10); // THRESH_ACT: 16 × 62.5 mg = 1 g
  accel.writeRegister(0x27, 0xFF); // ACT_INACT_CTL: all axes AC-coupled

  // Enable free-fall (bit2) and activity (bit4) interrupts on INT1
  accel.writeRegister(0x2E, 0x14); // INT_ENABLE: bit4=activity, bit2=freefall

  // Map both interrupts to INT1 pin (0 = INT1, 1 = INT2 for each bit)
  accel.writeRegister(0x2F, 0x00); // INT_MAP: all → INT1

  // Clear any pending interrupt by reading INT_SOURCE
  accel.readRegister(0x30);

  pinMode(INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), accelISR, RISING);
  Serial.println("ADXL345 interrupt ready");
}

void loop() {
  if (!intFired) return;
  intFired = false;

  uint8_t src = accel.readRegister(0x30); // INT_SOURCE — clears latch
  if (src & 0x04) Serial.println("Free-fall detected");
  if (src & 0x10) Serial.println("Activity detected");
}
