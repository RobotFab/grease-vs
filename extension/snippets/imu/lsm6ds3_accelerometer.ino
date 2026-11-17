// snippet: lsm6ds3_accelerometer
// tags: imu, accelerometer, accel, lsm6ds3, arduino_lsm6ds3, nano33iot, 6dof, g-force, xyz
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Read XYZ acceleration in g using the correct readAcceleration() API (NOT getAcceleration).
//   THIS IS THE NANO 33 IoT API — NOT compatible with MPU6050 or Romi32U4.
//   NOT for MPU6050: use mpu.getMotion6() instead. NOT for Romi: use LSM6 imu; imu.read(); imu.a.x
//   accelerationAvailable() and readAcceleration() ONLY work with Arduino_LSM6DS3 on Nano 33 IoT.
// prompts: LSM6DS3 accelerometer, Nano 33 IoT accel, readAcceleration API, XYZ g-force Nano IoT, built-in IMU acceleration
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, romi32u4
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

float ax, ay, az;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  // Halt here. `return` from setup() does NOT stop the sketch — loop() runs anyway, and
  // accelerationAvailable() on an IMU that never initialised simply stays false forever, so a
  // robot balancing on this data reads a permanent, perfectly level zero.
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
}

void loop() {
  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    IMU.readAcceleration(ax, ay, az);  // writes g-force into ax/ay/az (floats); NOT getMotion6()
    Serial.print("ax: "); Serial.print(ax);
    Serial.print(" ay: "); Serial.print(ay);
    Serial.print(" az: "); Serial.println(az);
  }
}
