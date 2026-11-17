// snippet: active_mode_250
// tags: active, serial, command, response, arduino_lsm6ds3
// boards: nano33iot
// library: Arduino_LSM6DS3
// description: Active sketch: IMU_STREAM <hz> streams complementary filter roll/pitch at given rate
// prompts: active, sketch, imu_stream, streams, complementary, filter
// verified: compile-only
// source: kd-generated (id 250)
// exact-include: Arduino_LSM6DS3.h

// Arduino_LSM6DS3 library is nano33iot-specific (built-in IMU)
#include <Arduino_LSM6DS3.h>

const float ALPHA = 0.96f;
float roll = 0.0f, pitch = 0.0f;
bool streaming = false;
unsigned long prevMs = 0;
unsigned long streamInterval = 1000;  // default 1 Hz

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  prevMs = millis();
  Serial.println("Send: IMU_STREAM <hz>  or  STOP");
}

void loop() {
  unsigned long now = millis();

  // Handle serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("IMU_STREAM")) {
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        int hz = command.substring(spaceIdx + 1).toInt();
        if (hz > 0) {
          streamInterval = 1000 / hz;
          streaming = true;
        }
      }
    } else if (command == "STOP") {
      streaming = false;
    }
  }

  if (now - prevMs >= streamInterval) {
    prevMs = now;

    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {  // LSM6DS3: true when a new accel sample is ready
      float ax, ay, az, gx, gy, gz;
      IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
      IMU.readGyroscope(gx, gy, gz);  // LSM6DS3: writes deg/s into the 3 float args

      // Accelerometer-based angles (degrees)
      float pitchAcc = atan2(ay, az) * 180.0f / PI;
      float rollAcc  = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0f / PI;

      // Complementary filter: gyro integrates angle, accel corrects drift
      float dt = streamInterval / 1000.0f;
      roll  = ALPHA * (roll  + gx * dt) + (1.0f - ALPHA) * rollAcc;
      pitch = ALPHA * (pitch + gy * dt) + (1.0f - ALPHA) * pitchAcc;

      if (streaming) {
        Serial.print("ROLL:"); Serial.print(roll);
        Serial.print(" PITCH:"); Serial.println(pitch);
      }
    }
  }
}
