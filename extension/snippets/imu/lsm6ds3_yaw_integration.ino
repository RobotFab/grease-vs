// snippet: lsm6ds3_yaw_integration
// tags: imu, gyroscope, lsm6ds3, arduino_lsm6ds3, nano33iot, yaw, integration, heading, gz
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Integrate gyroscope Z axis to estimate yaw angle, reset on D2 button press
// prompts: yaw from gyroscope, integrate gyro yaw, heading from gyro integration, gyroscope Z rotation, yaw angle tracking
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, romi32u4
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

const int BTN_PIN = 2;
unsigned long prevMs = 0;
unsigned long gyroMs = 0;
float yaw = 0.0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  pinMode(BTN_PIN, INPUT_PULLUP);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  gyroMs = millis();
}

void loop() {
  unsigned long now = millis();

  if (digitalRead(BTN_PIN) == LOW) {
    yaw = 0.0;
    gyroMs = now;
  }

  if (IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new gyro sample is ready
    float gx, gy, gz;
    IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args
    float dt = (now - gyroMs) / 1000.0;
    gyroMs = now;
    yaw += gz * dt;
  }

  if (now - prevMs >= 100) {
    prevMs = now;
    Serial.print("yaw: ");
    Serial.println(yaw);
  }
}
