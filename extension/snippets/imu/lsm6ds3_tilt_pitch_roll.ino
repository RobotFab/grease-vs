// snippet: lsm6ds3_tilt_pitch_roll
// tags: imu, accelerometer, lsm6ds3, arduino_lsm6ds3, nano33iot, tilt, pitch, roll, angle, static
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Static tilt sensing — pitch and roll from accelerometer only, printed every 200ms
// prompts: tilt pitch roll accelerometer, static tilt angle, pitch roll from accel only, inclination sensor Nano 33, accel-based angle
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, romi32u4
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

unsigned long prevMs = 0;
const unsigned long INTERVAL = 200;

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
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    float pitch = atan2(ay, az) * 180.0 / PI;
    float roll  = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
    Serial.print("pitch: "); Serial.print(pitch);
    Serial.print(" roll: "); Serial.println(roll);
  }
}
