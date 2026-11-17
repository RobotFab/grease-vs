// snippet: lis3dh_basic
// tags: imu, lis3dh, adafruit, accelerometer, accel, i2c, spi, 3dof, low-power
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit LIS3DH, Adafruit Unified Sensor
// description: Read XYZ acceleration from Adafruit LIS3DH breakout
// prompts: LIS3DH accelerometer, Adafruit LIS3DH, low-power 3-axis accel, accelerometer I2C SPI tilt
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688
// exact-include: Wire.h, Adafruit_LIS3DH.h, Adafruit_Sensor.h

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!lis.begin(0x18)) {
    Serial.println("LIS3DH not found");
    while (1);
  }
  lis.setRange(LIS3DH_RANGE_4_G);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  lis.read();
  sensors_event_t event;
  lis.getEvent(&event);  // unified Adafruit API: fills event.acceleration (m/s^2) / event.orientation (deg)
  Serial.print("ax: "); Serial.print(event.acceleration.x);
  Serial.print(" ay: "); Serial.print(event.acceleration.y);
  Serial.print(" az: "); Serial.println(event.acceleration.z);
}
