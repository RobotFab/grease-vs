// snippet: bno085_rotation_vector
// tags: imu, bno085, bno08x, quaternion, rotation, heading, fusion, adafruit, i2c
// boards: esp32, megaavr, nano33iot, rp2040
// library: Adafruit BNO08x
// description: Read absolute orientation quaternion from BNO085 using Adafruit BNO08x library
// prompts: BNO085 quaternion, BNO08x orientation, absolute heading sensor, quaternion rotation vector, fused IMU orientation, 9DOF attitude
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno08x, icm20948, icm42688, lis3dh
// exact-include: Adafruit_BNO08x.h

#include <Adafruit_BNO08x.h>

Adafruit_BNO08x bno;
sh2_SensorValue_t sensorValue;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  if (!bno.begin_I2C()) { Serial.println("BNO085 not found"); while (1); }
  bno.enableReport(SH2_ROTATION_VECTOR);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  prevMs = now;

  if (bno.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
      Serial.print("qr: "); Serial.print(sensorValue.un.rotationVector.real, 4);
      Serial.print(" qi: "); Serial.print(sensorValue.un.rotationVector.i, 4);
      Serial.print(" qj: "); Serial.print(sensorValue.un.rotationVector.j, 4);
      Serial.print(" qk: "); Serial.println(sensorValue.un.rotationVector.k, 4);
    }
  }
}
