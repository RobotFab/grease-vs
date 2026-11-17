// snippet: nano33iot_advanced_562
// tags: arduino_lsm6ds3
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Arduino_LSM6DS3
// description: Read gyroscope from the LSM6DS3 using Arduino_LSM6DS3
// prompts: gyroscope, lsm6ds3, arduino_lsm6ds3, integrate, angular, velocity
// verified: compile-only
// source: kd-generated (id 562)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

const int BTN_PIN = 4;
const unsigned long INTERVAL = 50;  // 20 Hz

float roll = 0.0, pitch = 0.0;
float lastGyroX = 0.0, lastGyroY = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  pinMode(BTN_PIN, INPUT_PULLUP);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    // Compute dt BEFORE updating prevMs
    // prevMs = now;  // This was moved to the end of the block
    float dt = (now - prevMs) / 1000.0;

    if (IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new gyro sample is ready
      float gx, gy, gz;
      IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args

      // Integrate angular velocity to get angle
      // Note: This is a simple integration without accel correction
      roll += (gx - lastGyroX) * dt * 180.0 / PI;
      pitch += (gy - lastGyroY) * dt * 180.0 / PI;

      lastGyroX = gx;
      lastGyroY = gy;

      Serial.print("roll: "); Serial.print(roll);
      Serial.print(" pitch: "); Serial.println(pitch);
    }

    if (digitalRead(BTN_PIN) == LOW) {
      roll = 0.0;
      pitch = 0.0;
      lastGyroX = 0.0;
      lastGyroY = 0.0;
    }

    // Update prevMs AFTER all calculations are done
    prevMs = now;
  }
}
