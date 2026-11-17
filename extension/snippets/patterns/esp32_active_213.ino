// snippet: esp32_active_213
// tags: esp32, active, serial, command, newping, mpu6050
// boards: esp32
// library: NewPing, MPU6050
// description: ESP32 active sketch: GET_DIST measures HC-SR04 pins 12/14, replies DIST:<cm>
// prompts: esp32, active, sketch, get_dist, measures, hc-sr04
// verified: compile-only
// source: kd-generated (id 213)
// exact-include: NewPing.h, Wire.h, MPU6050.h

#include <NewPing.h>
#include <Wire.h>
#include <MPU6050.h>

const int TRIGGER = 12;
const int ECHO    = 14;
const int MAX_CM  = 400;
NewPing sonar(TRIGGER, ECHO, MAX_CM);

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  Serial.println("Send: GET_DIST, READ_IMU, PING");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "GET_DIST") {
      int dist = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
      if (dist == 0) {
        Serial.println("DIST:out of range");
      } else {
        Serial.print("DIST:");
        Serial.println(dist);
      }
    } else if (cmd == "READ_IMU") {
      int16_t ax, ay, az;
      mpu.getMotion6(&ax, &ay, &az, NULL, NULL, NULL);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)
      Serial.print("ax,ay,az:");
      Serial.print(ax); Serial.print(",");
      Serial.print(ay); Serial.print(",");
      Serial.println(az);
    } else if (cmd == "PING") {
      Serial.println("PONG");
    }
  }
}
