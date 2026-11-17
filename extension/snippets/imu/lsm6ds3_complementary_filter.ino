// snippet: lsm6ds3_complementary_filter
// tags: imu, lsm6ds3, arduino_lsm6ds3, nano33iot, complementary, filter, fusion, pitch, roll, accel, gyro
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Complementary filter fusing accelerometer and gyroscope for stable pitch/roll
// prompts: complementary filter IMU, pitch roll fusion, gyro accel sensor fusion, stable tilt angle, fuse accel gyro pitch roll
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, romi32u4
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

const float ALPHA = 0.96;
float pitch = 0.0, roll = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  float dt = (now - prevMs) / 1000.0;
  prevMs = now;

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    float ax, ay, az, gx, gy, gz;
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args

    float pitchAcc = atan2(ay, az) * 180.0 / PI;
    float rollAcc  = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

    pitch = ALPHA * (pitch + gx * dt) + (1.0 - ALPHA) * pitchAcc;
    roll  = ALPHA * (roll  + gy * dt) + (1.0 - ALPHA) * rollAcc;

    Serial.print("pitch: "); Serial.print(pitch);
    Serial.print(" roll: "); Serial.println(roll);
  }
}
