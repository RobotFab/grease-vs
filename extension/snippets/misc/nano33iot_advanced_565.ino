// snippet: nano33iot_advanced_565
// tags: arduino_lsm6ds3
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Arduino_LSM6DS3
// description: Complementary filter: combine LSM6DS3 accel and gyro to compute stable pitch and roll
// prompts: complementary, filter, combine, lsm6ds3, accel, compute
// verified: compile-only
// source: kd-generated (id 565)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

const float ALPHA = 0.98;
float pitch = 0.0, roll = 0.0;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 20;  // 50 Hz

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
  if (now - prevMs < INTERVAL) return;
  
  // Compute dt BEFORE updating prevMs
  // prevMs = now;  // This was moved to the end of the block
  float dt = (now - prevMs) / 1000.0;

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    float ax, ay, az, gx, gy, gz;
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args

    float pitchAcc = atan2(ay, az) * 180.0 / PI;
    float rollAcc  = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

    // Complementary filter: blend gyro integration with accel correction
    // Note: dt is computed BEFORE prevMs is updated, so it's non-zero
    pitch = ALPHA * (pitch + gx * dt) + (1.0 - ALPHA) * pitchAcc;
    roll  = ALPHA * (roll  + gy * dt) + (1.0 - ALPHA) * rollAcc;

    Serial.print("pitch: "); Serial.print(pitch);
    Serial.print(" roll: "); Serial.println(roll);
  }

  // Update prevMs AFTER all calculations are done
  prevMs = now;
}
