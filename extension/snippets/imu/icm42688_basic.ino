// snippet: icm42688_basic
// tags: imu, icm42688, icm-42688, accelerometer, gyroscope, spi, i2c, sparkfun, invensense
// boards: esp32, nano33iot, rp2040, nano33ble
// library: SparkFun ICM-42688-P
// description: Read accel and gyro from ICM-42688-P using SparkFun library over SPI
// prompts: ICM-42688 IMU, high-speed accel gyro, ICM42688 SPI, SparkFun high-g IMU, ICM42688-P sensor
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, lis3dh
// exact-include: ICM42688.h

#include "ICM42688.h"

ICM42688 imu(SPI, 10);  // SPI with CS on pin 10
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  if (imu.begin() < 0) { Serial.println("ICM42688 init failed"); while (1); }
  imu.setAccelFS(ICM42688::gpm2);
  imu.setGyroFS(ICM42688::dps250);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  prevMs = now;

  imu.getAGT();
  Serial.print("ax: "); Serial.print(imu.accX(), 3);
  Serial.print(" ay: "); Serial.print(imu.accY(), 3);
  Serial.print(" az: "); Serial.print(imu.accZ(), 3);
  Serial.print("  gx: "); Serial.println(imu.gyrX(), 2);
}
