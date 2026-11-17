// snippet: adxl345_basic
// tags: imu, adxl345, accelerometer, i2c, adafruit, 3dof, tilt, low-power
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit ADXL345
// description: Read XYZ acceleration from ADXL345 using Adafruit library
// prompts: ADXL345 accelerometer, 3-axis acceleration, tilt detection ADXL, Adafruit ADXL345, accelerometer I2C XYZ
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Wire.h, Adafruit_Sensor.h, Adafruit_ADXL345_U.h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel(12345);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!accel.begin()) { Serial.println("ADXL345 not found"); while (1); }
  accel.setRange(ADXL345_RANGE_4_G);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  sensors_event_t event;
  accel.getEvent(&event);  // unified API: fills event.acceleration.{x,y,z} in m/s²
  Serial.print("X: "); Serial.print(event.acceleration.x);
  Serial.print(" Y: "); Serial.print(event.acceleration.y);
  Serial.print(" Z: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2");
}
