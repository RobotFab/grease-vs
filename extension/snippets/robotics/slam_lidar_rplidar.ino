// snippet: slam_lidar_rplidar
// tags: slam, lidar, rplidar, a1, mapping, polar, scan, 360, serial
// boards: esp32, megaavr, nano33iot
// verified: compile-only
// library: RPLidar
// description: Collect 360° RPLIDAR A1 scan points and print polar/XY data for mapping
// prompts: RPLIDAR SLAM, A1 lidar 360 scan, polar map lidar, RPLidar scan points, lidar occupancy mapping
// exact-include: RPLidar.h

#include <RPLidar.h>

RPLidar lidar;

// RPLIDAR A1 needs Serial1 (Mega: 18/19, ESP32: GPIO16/17)
#define LIDAR_SERIAL  Serial1
#define LIDAR_MOTOR   3   // PWM pin to control RPLIDAR motor

void setup() {
  Serial.begin(115200);
  LIDAR_SERIAL.begin(115200);
  lidar.begin(LIDAR_SERIAL);  // RPLidar: start on the given serial port/baud

  pinMode(LIDAR_MOTOR, OUTPUT);
  analogWrite(LIDAR_MOTOR, 200);  // start motor
  delay(1000);

  if (IS_OK(lidar.startScan())) {  // RPLidar: begin 360-degree scan
    Serial.println("RPLIDAR scanning");
  } else {
    Serial.println("RPLIDAR start failed"); while (1);
  }
}

void loop() {
  if (IS_OK(lidar.waitPoint())) {  // RPLidar: blocks until the next scan point is available
    float dist  = lidar.getCurrentPoint().distance;   // mm
    float angle = lidar.getCurrentPoint().angle;      // degrees
    bool  start = lidar.getCurrentPoint().startBit;   // true = new revolution

    if (start) Serial.println("--- new revolution ---");

    if (dist > 0) {
      float rad = angle * PI / 180.0f;
      float x = dist * cos(rad);
      float y = dist * sin(rad);
      Serial.print("a="); Serial.print(angle, 1);
      Serial.print(" d="); Serial.print(dist, 0);
      Serial.print(" x="); Serial.print(x, 0);
      Serial.print(" y="); Serial.println(y, 0);
    }
  }
}
