// snippet: nano33iot_294
// tags: nano33iot, samd, imu, wifi, arduino_lsm6ds3
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Arduino Nano 33 IoT: tilt detection using LSM6DS3
// prompts: arduino, detection, lsm6ds3, include, arduino_lsm6ds3, begin()
// verified: compile-only
// source: kd-generated (id 294)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

float ax, ay, az, roll, pitch;
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
    roll = atan2(ay, az) * 57.3;
    pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 57.3;
    if (abs(roll) > 30 || abs(pitch) > 30) {
      Serial.println("TILT");
    } else {
      Serial.println("LEVEL");
    }
  }
}
