// snippet: bno055_euler
// tags: imu, bno055, adafruit, euler, orientation, heading, pitch, roll, yaw, 9dof, fusion
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit BNO055, Adafruit Unified Sensor
// description: Read fused Euler angles (heading/pitch/roll) from Adafruit BNO055
// prompts: BNO055 IMU, absolute orientation, heading pitch roll yaw, Euler angles, 9DOF fusion, compass heading, Adafruit BNO055
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, adxl345, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_BNO055.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!bno.begin()) {
    Serial.println("BNO055 not found");
    while (1);
  }
  bno.setExtCrystalUse(true);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  sensors_event_t event;
  bno.getEvent(&event);  // unified API: fills event.orientation.{x=heading, y=pitch, z=roll} in degrees
  Serial.print("heading: "); Serial.print(event.orientation.x);
  Serial.print(" pitch: "); Serial.print(event.orientation.y);
  Serial.print(" roll: "); Serial.println(event.orientation.z);
}
