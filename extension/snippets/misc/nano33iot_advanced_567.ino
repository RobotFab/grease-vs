// snippet: nano33iot_advanced_567
// tags: arduino_lsm6ds3
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Arduino_LSM6DS3
// description: Step counter: use LSM6DS3 acceleration to detect steps (peak detection on vertical axis, threshold 1
// prompts: counter, lsm6ds3, acceleration, detect, steps, (peak
// verified: compile-only
// source: kd-generated (id 567)
// exact-include: Arduino_LSM6DS3.h

#include <Arduino_LSM6DS3.h>

const float STEP_THRESHOLD = 1.2;
const unsigned long MIN_TIME_BETWEEN_STEPS = 300;

float ax, ay, az;
unsigned long lastStepTime = 0;
int stepCount = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  Serial.println("IMU ready");
}

void loop() {
  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    float verticalAccel = ay;

    if (verticalAccel > STEP_THRESHOLD && (millis() - lastStepTime) > MIN_TIME_BETWEEN_STEPS) {
      stepCount++;
      lastStepTime = millis();
    }
  }

  static unsigned long printTime = 0;
  if (millis() - printTime >= 1000) {
    printTime = millis();
    Serial.print("Step Count: ");
    Serial.println(stepCount);
  }
}
