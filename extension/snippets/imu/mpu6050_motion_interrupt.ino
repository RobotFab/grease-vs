// snippet: mpu6050_motion_interrupt
// tags: imu, mpu6050, interrupt, motion-detection, irq, i2c, wake-on-motion, activity
// boards: avr, esp32, megaavr, nano33iot, rp2040
// verified: compile-only
// library: MPU6050 (jrowberg i2cdevlib)
// description: MPU6050 motion detection interrupt — fires when acceleration delta exceeds threshold.
//   setMotionDetectionThreshold(uint8_t mg) — 1 LSB = 2 mg; 2 = ~4 mg sensitivity.
//   setMotionDetectionDuration(uint8_t ms) — how many ms motion must persist to trigger.
//   setIntMotionEnabled(true) arms the INT pin; getIntMotionStatus() reads the latch.
//   Useful for wake-on-motion: put AVR to sleep, wake via MPU6050 INT interrupt.
//   Wire.begin() is REQUIRED before mpu.initialize().
// prompts: MPU6050 motion interrupt, MPU6050 motion detection, wake on motion MPU6050, activity detection MPU6050, MPU6050 IRQ motion, MPU6050 vibration detection, accelerometer motion trigger
// not-tags: mpu9250, lsm6ds3, adxl345, bno055, bno085, icm20948, icm42688, lis3dh
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const uint8_t INT_PIN        = 2;  // MPU6050 INT → Arduino pin 2
const uint8_t MOT_THRESHOLD  = 2;  // 1 LSB = 2 mg; 2 = ~4 mg
const uint8_t MOT_DURATION   = 20; // ms motion must persist

volatile bool motionDetected = false;

void motISR() {
  motionDetected = true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  // Halt here. `return` from setup() would skip every line below — the motion thresholds,
  // the interrupt config and attachInterrupt() — while loop() carries on regardless.
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found — check wiring and AD0");
    while (1);
  }

  // Configure motion detection
  mpu.setMotionDetectionThreshold(MOT_THRESHOLD);
  mpu.setMotionDetectionDuration(MOT_DURATION);
  // Route motion interrupt to INT pin
  mpu.setInterruptMode(MPU6050_INTMODE_ACTIVEHIGH);
  mpu.setInterruptDrive(MPU6050_INTDRV_PUSHPULL);
  mpu.setInterruptLatch(MPU6050_INTLATCH_WAITCLEAR);
  mpu.setInterruptLatchClear(MPU6050_INTCLEAR_STATUSREAD);
  mpu.setIntMotionEnabled(true);

  pinMode(INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), motISR, RISING);
  Serial.println("MPU6050 motion detect ready");
}

void loop() {
  if (!motionDetected) return;
  motionDetected = false;

  if (mpu.getIntMotionStatus()) {
    Serial.println("Motion detected");
  }
}
