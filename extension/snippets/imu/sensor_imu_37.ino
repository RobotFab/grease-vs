// snippet: sensor_imu_37
// tags: imu, accelerometer, gyroscope, i2c, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: MPU6050 pose control: compute roll and pitch from complementary filter
// prompts: mpu6050, control, compute, pitch, complementary, filter
// verified: compile-only
// source: kd-generated (id 37)
// exact-include: Wire.h, Servo.h

#include <Wire.h>
#include <Servo.h>

// MPU6050 I2C address
#define MPU6050_ADDR 0x68

// Servos
Servo panServo;
Servo tiltServo;

// MPU6050 registers
const byte ACCEL_XOUT_H = 0x3B;
const byte GYRO_XOUT_H = 0x43;
const byte PWR_MGMT_1 = 0x6B;

// Variables
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float roll = 0.0, pitch = 0.0;
float rollAngle, pitchAngle;
float dt = 0.05; // 20Hz update rate (50ms)
unsigned long lastUpdate = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  // Initialize MPU6050 — wake from sleep (PWR_MGMT_1 = 0)
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);

  // Initialize servos
  panServo.attach(9);
  tiltServo.attach(10);

  // Set initial servo positions
  panServo.write(90);
  tiltServo.write(90);
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdate >= 50) {
    lastUpdate = now;

    // Read raw data from MPU6050 (6 accel + 2 temp + 6 gyro = 14 bytes)
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU6050_ADDR, (uint8_t)14, (uint8_t)true);

    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();
    Wire.read() << 8 | Wire.read();  // skip temperature (2 bytes)
    int16_t gx = Wire.read() << 8 | Wire.read();
    int16_t gy = Wire.read() << 8 | Wire.read();
    int16_t gz = Wire.read() << 8 | Wire.read();

    // Convert raw data to actual values
    accelX = ax / 16384.0;  // ±2g range: 16384 LSB/g
    accelY = ay / 16384.0;
    accelZ = az / 16384.0;
    gyroX = gx / 131.0;     // ±250°/s range: 131 LSB/(°/s)
    gyroY = gy / 131.0;
    gyroZ = gz / 131.0;

    // Accel-derived angles in DEGREES (atan2 returns radians → convert)
    float accelRoll  = atan2(accelY, accelZ) * 180.0 / PI;
    float accelPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0 / PI;

    // Complementary filter — all terms in DEGREES.
    // The original mixed gyro deg/s with accel radians, then map()'d radians
    // as if they were degrees — unit mismatch corrupted the fusion.
    roll  = 0.98 * (roll  + gyroX * dt) + 0.02 * accelRoll;
    pitch = 0.98 * (pitch + gyroY * dt) + 0.02 * accelPitch;

    // Convert angles to servo positions (degrees → 0-180)
    rollAngle  = constrain(roll  + 90, 0, 180);
    pitchAngle = constrain(pitch + 90, 0, 180);

    // Set servo positions
    panServo.write((int)rollAngle);
    tiltServo.write((int)pitchAngle);
  }
}
