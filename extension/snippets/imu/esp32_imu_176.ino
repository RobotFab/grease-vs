// snippet: esp32_imu_176
// tags: esp32, imu, mpu6050, i2c
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: ESP32: read MPU6050 accel and gyro over I2C
// prompts: esp32, mpu6050, accel, declare, float, roll=0
// verified: compile-only
// source: kd-generated (id 176)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
float roll = 0.0, pitch = 0.0;
float accelRoll, accelPitch, gyroX, gyroY;
float alpha = 0.98;  // Complementary filter coefficient
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);
  }
  Serial.println("MPU6050 ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;  // 50Hz update rate
  prevMs = now;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  // Accel-derived angles
  accelRoll  = atan2(ay, az) * 180.0 / PI;
  accelPitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  // Complementary filter
  // MPU6050 gyro raw → deg/s: divide by 131.0 (at ±250°/s default range).
  // Using gx * 0.001 is a ~7.6x scale error — the gyro term barely contributes.
  float dt = 0.02;  // 50Hz → 20ms per sample
  roll  = alpha * (roll  + (gx / 131.0) * dt) + (1.0 - alpha) * accelRoll;
  pitch = alpha * (pitch + (gy / 131.0) * dt) + (1.0 - alpha) * accelPitch;

  Serial.print("roll: ");  Serial.print(roll,  2);
  Serial.print("  pitch: "); Serial.println(pitch, 2);
}
