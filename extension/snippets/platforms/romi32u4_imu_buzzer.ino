// snippet: romi32u4_imu_buzzer
// tags: romi, pololu, romi32u4, imu, lsm6ds3, lsm6, buzzer, beep, inertial, accelerometer, gyroscope, roll, pitch, complementary, filter, fusion, angle, tilt
// boards: romi32u4
// library: Romi32U4, LSM6
// description: Romi 32U4 onboard IMU (LSM6DS33) and buzzer.
//   IMU class is LSM6 from the separate LSM6 library — NOT part of Romi32U4.h.
//   NOT: Romi32U4IMU — that class does not exist. NOT: #include <Romi32U4IMU.h> — no such header.
//   NOT: Arduino_LSM6DS3 / IMU.accelerationAvailable() — that is the Nano 33 IoT API, not Romi.
//   Access: imu.a.x, imu.a.y, imu.a.z (int16_t accel), imu.g.x, imu.g.y, imu.g.z (int16_t gyro).
//   Buzzer: Romi32U4Buzzer — buzzer.play("..."), buzzer.isPlaying() — DOES exist.
// prompts: Romi 32U4 IMU buzzer, Romi LSM6 accelerometer gyroscope, Romi32U4 IMU read accel gyro, Romi LSM6DS33 serial, Romi inertial sensor, Romi imu.read imu.a.x imu.g.z, Romi roll pitch IMU, Romi complementary filter, Romi32U4 pitch roll angles, Romi LSM6 tilt angle, Romi fuse accel gyro
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, nano33iot, arduino-lsm6ds3
// exact-include: Romi32U4.h, Wire.h, LSM6.h

#include <Romi32U4.h>
#include <Wire.h>
#include <LSM6.h>

// NOT: Romi32U4IMU imu;  — no such class. Use LSM6 imu instead:
LSM6 imu;
Romi32U4Buzzer buzzer;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!imu.init()) {
    Serial.println("IMU init failed");
    while (1);
  }
  imu.enableDefault();
  buzzer.play(">g32>>c32");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  imu.read();  // Romi IMU: reads accel+gyro into ax/ay/az/gx/gy/gz
  Serial.print("ax: "); Serial.print(imu.a.x);
  Serial.print(" ay: "); Serial.print(imu.a.y);
  Serial.print(" az: "); Serial.print(imu.a.z);
  Serial.print(" gz: "); Serial.println(imu.g.z);
}
