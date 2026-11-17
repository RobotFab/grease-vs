// snippet: mpu6050_freefall_interrupt
// tags: imu, mpu6050, interrupt, freefall, free-fall, fall-detection, irq, i2c, motion
// boards: avr, esp32, megaavr, nano33iot, rp2040
// verified: compile-only
// library: MPU6050 (jrowberg i2cdevlib)
// description: MPU6050 free-fall detection via hardware interrupt on pin 2 (INT0).
//   jrowberg API: setFreefallDetectionThreshold(uint8_t mg) — 1 LSB = 1 mg; 17 = ~170 mg.
//   setFreefallDetectionDuration(uint8_t ms) — duration all axes must be below threshold.
//   setIntFreefallEnabled(true) arms the INT pin; getIntFreefallStatus() reads the latch.
//   INT pin is active HIGH push-pull by default — wire directly to Arduino digital pin 2.
//   Wire.begin() is REQUIRED before mpu.initialize() — the library does NOT call it.
//   NOT for zero-motion detection (use setZeroMotion* instead).
// prompts: MPU6050 free fall interrupt, free-fall detection MPU6050, MPU6050 fall detection interrupt, MPU6050 IRQ pin 2, MPU6050 drop detection, accelerometer free fall, MPU6050 impact interrupt
// not-tags: mpu9250, lsm6ds3, adxl345, bno055, bno085, icm20948, icm42688, lis3dh
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const uint8_t INT_PIN = 2;       // MPU6050 INT → Arduino pin 2 (INT0)
const uint8_t FF_THRESHOLD = 17; // ~170 mg — lower = more sensitive
const uint8_t FF_DURATION  = 5;  // ms all axes must be below threshold

volatile bool freeFallDetected = false;

void ffISR() {
  freeFallDetected = true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);
  }

  // Configure free-fall detection
  mpu.setFreefallDetectionThreshold(FF_THRESHOLD);
  mpu.setFreefallDetectionDuration(FF_DURATION);
  mpu.setIntFreefallEnabled(true);

  // Route free-fall interrupt to INT pin
  mpu.setInterruptMode(MPU6050_INTMODE_ACTIVEHIGH);
  mpu.setInterruptDrive(MPU6050_INTDRV_PUSHPULL);
  mpu.setInterruptLatch(MPU6050_INTLATCH_WAITCLEAR);
  mpu.setInterruptLatchClear(MPU6050_INTCLEAR_STATUSREAD);
  mpu.setIntFreefallEnabled(true);

  pinMode(INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), ffISR, RISING);
  Serial.println("MPU6050 free-fall ready");
}

void loop() {
  if (!freeFallDetected) return;
  freeFallDetected = false;

  if (mpu.getIntFreefallStatus()) {
    Serial.println("FREE FALL detected");
  }
}
