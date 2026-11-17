// snippet: lsm6ds3_accel_magnitude
// tags: imu, accelerometer, lsm6ds3, arduino_lsm6ds3, nano33iot, magnitude, shock, impact, total
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Read acceleration magnitude (sqrt of x^2+y^2+z^2) at 100 Hz
// prompts: acceleration magnitude, total g-force, shock detection, impact sensing, sqrt X squared Y squared Z squared accel
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, romi32u4
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  prevMs = now;

  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    float mag = sqrt(ax * ax + ay * ay + az * az);
    Serial.print("magnitude: ");
    Serial.println(mag);
  }
}
