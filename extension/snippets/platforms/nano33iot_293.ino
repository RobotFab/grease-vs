// snippet: nano33iot_293
// tags: nano33iot, samd, imu, wifi, arduino_lsm6ds3
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Arduino Nano 33 IoT: read LSM6DS3 accelerometer and gyroscope
// prompts: arduino, lsm6ds3, accelerometer, gyroscope, include, arduino_lsm6ds3
// verified: compile-only
// source: kd-generated (id 293)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

float ax, ay, az, gx, gy, gz;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) { Serial.println("IMU init failed"); while (1); }
  Serial.println("IMU ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    Serial.print("ax: "); Serial.print(ax);
    Serial.print(" ay: "); Serial.print(ay);
    Serial.print(" az: "); Serial.println(az);
  }
  if (IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new gyro sample is ready
    IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args
    Serial.print("gx: "); Serial.print(gx);
    Serial.print(" gy: "); Serial.print(gy);
    Serial.print(" gz: "); Serial.println(gz);
  }
}
