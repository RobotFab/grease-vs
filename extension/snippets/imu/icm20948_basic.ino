// snippet: icm20948_basic
// tags: imu, icm20948, icm, sparkfun, 9dof, accelerometer, gyroscope, magnetometer
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: SparkFun ICM-20948 IMU
// description: Read accel, gyro, and magnetometer from SparkFun ICM-20948 breakout
// prompts: ICM-20948 9DOF, SparkFun ICM IMU, accelerometer gyroscope magnetometer, 9-axis IMU, ICM20948 compass
// not-tags: mpu6050, mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm42688, lis3dh
// exact-include: Wire.h, ICM_20948.h

#include <Wire.h>
#include <ICM_20948.h>

ICM_20948_I2C myICM;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  Wire.begin();
  Wire.setClock(400000);
  // Bounded retry — without a timeout, hangs forever in setup() if the IMU
  // is absent or miswired, with no diagnostic output.
  bool init = false;
  unsigned long initStart = millis();
  while (!init && millis() - initStart < 5000) {
    myICM.begin(Wire, 1);
    if (myICM.status == ICM_20948_Stat_Ok) init = true;
    else delay(500);
  }
  if (!init) {
    Serial.println("ICM-20948 init failed — check wiring");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  if (myICM.dataReady()) {
    myICM.getAGMT();
    Serial.print("ax: "); Serial.print(myICM.accX());
    Serial.print(" gx: "); Serial.print(myICM.gyrX());
    Serial.print(" mx: "); Serial.println(myICM.magX());
  }
}
