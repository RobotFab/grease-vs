// snippet: mpu6050_basic
// tags: imu, mpu6050, accelerometer, gyroscope, i2c, wire, 6dof, motion, inertial
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050 (jrowberg i2cdevlib)
// description: Read raw accelerometer and gyroscope XYZ from MPU6050 over I2C using jrowberg i2cdevlib.
//   API: mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz) — ALWAYS 6 pointer arguments, all int16_t.
//   There is NO 3-argument overload. Even if you only need accel, you must pass all 6 pointers.
//   Wire.begin() is required — the library does not call it internally.
//   WRONG METHODS — do NOT use these with MPU6050 (they are LSM6DS3 / Nano 33 IoT API):
//     mpu.accelerationAvailable()  mpu.gyroscopeAvailable()
//     mpu.readAcceleration()       mpu.readGyroscope()
//     mpu.getMotion6D()            IMU.begin()
//   The only correct call is: mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz)
// prompts: MPU6050 accelerometer gyroscope, read MPU6050 I2C, 6DOF IMU MPU6050, getMotion6, MPU6050 accel gyro XYZ, MPU6050 tilt pitch roll, MPU6050 yaw integration, MPU6050 complementary filter, MPU6050 angle estimation, MPU6050 gyroscope Z yaw, MPU6050 accelerometer pitch roll static tilt, MPU6050 stream serial plotter, MPU6050 integrate gyroscope heading, step counter MPU6050, pedometer MPU6050, count steps MPU6050, accel magnitude MPU6050, acceleration magnitude threshold, detect motion MPU6050, impact detection MPU6050, shake detection
// not-tags: mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  // Halt here. `return` from setup() does NOT stop the sketch — loop() runs anyway and
  // reads a device that is not there, printing plausible-looking zeros forever.
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found — check wiring and AD0");
    while (1);
  }
  Serial.println("MPU6050 ready");
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t < 100) return;
  t = millis();

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // 6 pointer args (ax..gz), NO 3-arg overload; Wire.begin() is required (lib won't call it)

  Serial.print("ax: "); Serial.print(ax);
  Serial.print("  ay: "); Serial.print(ay);
  Serial.print("  az: "); Serial.print(az);
  Serial.print("  gx: "); Serial.print(gx);
  Serial.print("  gy: "); Serial.print(gy);
  Serial.print("  gz: "); Serial.println(gz);
}
