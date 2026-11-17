// snippet: lsm6dsox_nano
// tags: imu, lsm6dsox, accelerometer, gyroscope, nano33iot, nano33ble, onboard, 6dof
// boards: nano33iot, nano33ble
// library: Arduino_LSM6DSOX
// description: NANO 33 IoT / BLE ONLY — reads the onboard LSM6DSOX via Arduino_LSM6DSOX.
//   This library talks to the chip hard-wired on the board; it will NOT work with an external
//   LSM6DSOX wired over I2C. For an external sensor use lsm6dsox_i2c instead.
// prompts: LSM6DSOX Nano 33, onboard IMU Nano 33 IoT BLE, Arduino_LSM6DSOX library, built-in 6DOF Nano, LSM6DSOX Arduino library
// not-tags: mpu6050, mpu9250, lsm6ds3, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Arduino_LSM6DSOX.h

#include <Arduino_LSM6DSOX.h>

float ax, ay, az, gx, gy, gz;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) { Serial.println("LSM6DSOX init failed"); while (1); }
  Serial.print("Accel sample rate: "); Serial.println(IMU.accelerationSampleRate());
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    Serial.print("A: "); Serial.print(ax); Serial.print(" "); Serial.print(ay); Serial.print(" "); Serial.println(az);
  }
  if (IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new gyro sample is ready
    IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args
    Serial.print("G: "); Serial.print(gx); Serial.print(" "); Serial.print(gy); Serial.print(" "); Serial.println(gz);
  }
}
