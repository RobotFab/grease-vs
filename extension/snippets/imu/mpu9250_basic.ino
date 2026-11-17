// snippet: mpu9250_basic
// tags: imu, mpu9250, mpu-9250, accelerometer, gyroscope, magnetometer, i2c, 9dof, invensense
// boards: avr, esp32, megaavr, nano33iot, rp2040, nano33ble
// library: MPU9250
// description: Read 9-DOF data (accel, gyro, mag) from MPU-9250 using hideakitai/MPU9250
// prompts: MPU-9250 9DOF, MPU9250 accelerometer gyroscope magnetometer, 9-axis IMU, compass accel gyro
// not-tags: mpu6050, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Wire.h, MPU9250.h

#include <Wire.h>
#include "MPU9250.h"

MPU9250 mpu;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  MPU9250Setting setting;
  setting.accel_fs_sel = ACCEL_FS_SEL::A8G;
  setting.gyro_fs_sel  = GYRO_FS_SEL::G500DPS;
  if (!mpu.setup(0x68, setting)) { Serial.println("MPU9250 init failed"); while (1); }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  if (!mpu.update()) return;  // reads all 9 axes; getters below read them out
  Serial.print("ax: "); Serial.print(mpu.getAccX(), 2);   // g
  Serial.print(" ay: "); Serial.print(mpu.getAccY(), 2);
  Serial.print(" az: "); Serial.print(mpu.getAccZ(), 2);
  Serial.print("  gx: "); Serial.print(mpu.getGyroX(), 3); // dps
  Serial.print("  mx: "); Serial.println(mpu.getMagX(), 1); // uT
}
