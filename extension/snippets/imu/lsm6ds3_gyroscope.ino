// snippet: lsm6ds3_gyroscope
// tags: imu, gyroscope, gyro, lsm6ds3, arduino_lsm6ds3, nano33iot, rotation, dps, angular
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Read XYZ gyroscope in degrees/s using the correct readGyroscope() API.
//   THIS IS THE NANO 33 IoT API — NOT compatible with MPU6050 or Romi32U4.
//   NOT for MPU6050: use mpu.getMotion6() instead. NOT for Romi: use LSM6 imu; imu.read(); imu.g.z
//   gyroscopeAvailable() and readGyroscope() ONLY work with Arduino_LSM6DS3 on Nano 33 IoT.
// prompts: LSM6DS3 gyroscope, Nano 33 IoT gyro, readGyroscope API, angular velocity degrees per second, rotation rate Nano
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, romi32u4
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

float gx, gy, gz;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
}

void loop() {
  if (IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new gyro sample is ready
    IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args
    Serial.print("gx: "); Serial.print(gx);
    Serial.print(" gy: "); Serial.print(gy);
    Serial.print(" gz: "); Serial.println(gz);
  }
}
