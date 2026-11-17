// snippet: mpu6050_complementary_filter
// tags: imu, mpu6050, complementary, filter, fusion, pitch, roll, accel, gyro, angle, tilt
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050 (jrowberg i2cdevlib)
// description: Complementary filter for stable pitch/roll from MPU6050 accel+gyro fusion
//   getMotion6 ALWAYS takes 6 int16_t* arguments. Convert to float AFTER reading.
//   Accel angles: atan2 for pitch/roll. Gyro: integrate rate × dt.
//   Filter: angle = ALPHA*(angle + gyroRate*dt) + (1-ALPHA)*accelAngle
// prompts: MPU6050 complementary filter, MPU6050 pitch roll, MPU6050 tilt angle, MPU6050 sensor fusion, fuse accel gyro MPU6050, stable angle MPU6050, MPU6050 orientation, MPU6050 roll pitch yaw
// not-tags: lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh, mpu9250
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;   // getMotion6 needs int16_t*, not float*
float roll = 0.0, pitch = 0.0;
const float ALPHA = 0.98;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);
  }
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;   // 50 Hz
  float dt = (now - prevMs) / 1000.0;
  prevMs = now;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  // Accel-derived angles (noisy but no drift)
  float accelRoll  = atan2((float)ay, (float)az) * 180.0 / PI;
  float accelPitch = atan2(-(float)ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / PI;

  // Gyro rates in deg/s (±250°/s default: 131 LSB/(°/s))
  float gyroRateX = gx / 131.0;
  float gyroRateY = gy / 131.0;

  // Complementary filter: trust gyro short-term, accel long-term
  roll  = ALPHA * (roll  + gyroRateX * dt) + (1.0 - ALPHA) * accelRoll;
  pitch = ALPHA * (pitch + gyroRateY * dt) + (1.0 - ALPHA) * accelPitch;

  Serial.print("roll: ");  Serial.print(roll, 2);
  Serial.print("  pitch: "); Serial.println(pitch, 2);
}
