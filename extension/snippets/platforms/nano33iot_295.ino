// snippet: nano33iot_295
// tags: nano33iot, samd, imu, wifi, arduino_lsm6ds3
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Arduino Nano 33 IoT: serial plotter for LSM6DS3
// prompts: arduino, serial, plotter, lsm6ds3, include, arduino_lsm6ds3
// verified: compile-only
// source: kd-generated (id 295)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

float ax, ay, az;
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
  if (now - prevMs >= 20) {  // 50Hz
    prevMs = now;
    if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
      IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
      Serial.print("ax:"); Serial.print(ax);
      Serial.print(" ay:"); Serial.print(ay);
      Serial.print(" az:"); Serial.println(az);
    }
  }
}
