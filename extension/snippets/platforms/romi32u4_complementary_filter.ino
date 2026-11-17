// snippet: romi32u4_complementary_filter
// tags: romi, romi32u4, pololu, imu, lsm6, lsm6ds3, complementary, filter, fusion, pitch, roll, accel, gyro, angle, tilt, inertial, orientation
// boards: romi32u4
// library: Romi32U4, LSM6
// description: Romi 32U4 complementary filter for roll and pitch using the onboard LSM6DS33 IMU.
//   Uses LSM6 library (NOT Arduino_LSM6DS3 — that is Nano 33 IoT only).
//   Raw values from imu.a.x/y/z are int16_t; divide by 16384.0 to get g. Gyro imu.g.x/y/z
//   are int16_t in mdps*10; divide by 114.3 to get deg/s (LSM6DS33 at 245 dps range).
//   alpha controls filter blend: higher keeps more gyro integration, lower trusts accel more.
// prompts: Romi complementary filter, Romi roll pitch IMU, Romi32U4 pitch roll angles, Romi LSM6 tilt angle, Romi fuse accel gyro, Romi LSM6DS33 roll pitch, Romi angle estimation, Romi IMU orientation
// not-tags: nano33iot, arduino-lsm6ds3, mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688
// exact-include: Romi32U4.h, Wire.h, LSM6.h

#include <Romi32U4.h>
#include <Wire.h>
#include <LSM6.h>

LSM6 imu;

// Complementary filter constant: 0.98 → 98% gyro, 2% accel correction
const float ALPHA = 0.98f;

float roll  = 0.0f;  // deg
float pitch = 0.0f;  // deg

unsigned long prevMs = 0;
unsigned long printMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!imu.init()) {
    Serial.println("IMU init failed");
    while (1);
  }
  imu.enableDefault();
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  float dt = (now - prevMs) * 0.001f;  // seconds
  prevMs = now;

  imu.read();  // Romi IMU: reads accel+gyro into ax/ay/az/gx/gy/gz

  // Scale raw accel to g (sensitivity 2g → 16384 LSB/g)
  float ax = imu.a.x / 16384.0f;
  float ay = imu.a.y / 16384.0f;
  float az = imu.a.z / 16384.0f;

  // Scale raw gyro to deg/s (sensitivity 245 dps → 114.3 LSB/(deg/s) for LSM6DS33)
  // Note: MPU6050 uses 131.0; LSM6DS33 at 245 dps range uses ~114.3
  float gx = imu.g.x / 114.3f;
  float gy = imu.g.y / 114.3f;

  // Accel-derived tilt angles (deg)
  float accelRoll  = atan2f(ay, az) * 180.0f / M_PI;
  float accelPitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / M_PI;

  // Complementary filter: blend gyro integration with accel correction
  roll  = ALPHA * (roll  + gx * dt) + (1.0f - ALPHA) * accelRoll;
  pitch = ALPHA * (pitch + gy * dt) + (1.0f - ALPHA) * accelPitch;

  // Print every 200 ms
  if (now - printMs >= 200) {
    printMs = now;
    Serial.print("Roll: ");  Serial.print(roll,  2);
    Serial.print(" deg  Pitch: "); Serial.print(pitch, 2);
    Serial.println(" deg");
  }
}
