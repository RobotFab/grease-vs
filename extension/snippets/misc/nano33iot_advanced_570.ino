// snippet: nano33iot_advanced_570
// tags: arduino_lsm6ds3
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Arduino_LSM6DS3
// description: Gesture detection: read LSM6DS3 acceleration at 100 Hz
// prompts: gesture, detection, lsm6ds3, acceleration, detect, shake
// verified: compile-only
// source: kd-generated (id 570)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

#define SHAKE_THRESHOLD 2.5
#define TILT_THRESHOLD 0.7
#define SAMPLE_COUNT 3

unsigned long prevMs = 0;
int shakeCount = 0;
int tiltLeftCount = 0;
int tiltRightCount = 0;

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

    if (mag > SHAKE_THRESHOLD) {
      shakeCount++;
      if (shakeCount >= SAMPLE_COUNT) {
        Serial.println("SHAKE");
        shakeCount = 0;
      }
    } else {
      shakeCount = 0;
    }

    if (ax < -TILT_THRESHOLD) {
      tiltLeftCount++;
      if (tiltLeftCount >= SAMPLE_COUNT) {
        Serial.println("LEFT");
        tiltLeftCount = 0;
      }
    } else {
      tiltLeftCount = 0;
    }

    if (ax > TILT_THRESHOLD) {
      tiltRightCount++;
      if (tiltRightCount >= SAMPLE_COUNT) {
        Serial.println("RIGHT");
        tiltRightCount = 0;
      }
    } else {
      tiltRightCount = 0;
    }
  }
}
