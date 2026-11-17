// snippet: pico_mpu6050
// tags: pico, rp2040, raspberry-pi, mpu6050, imu, i2c, accelerometer, gyroscope, inertial, motion, accel, gyro, wire
// boards: rp2040
// library: MPU6050 (jrowberg i2cdevlib)
// description: RPi Pico: MPU-6050 over I2C using GP4=SDA, GP5=SCL. Prints accel and gyro.
//   On RP2040, Wire uses GPIO pins set via Wire.setSDA()/Wire.setSCL() BEFORE Wire.begin().
//   Default I2C address: 0x68 (AD0 low). Use 0x69 if AD0 tied high.
//   getMotion6(&ax,&ay,&az,&gx,&gy,&gz): all 6 axes in one call — 6 int16_t args required.
//   NOT: getMotion6(ax,ay,az) — that is the wrong overload; will not compile.
//   NOT: IMU.readAcceleration() — that is the Nano 33 IoT / Arduino_LSM6DS3 API.
//   Raw accel units: 16384 LSB/g (±2g default). Raw gyro: 131 LSB/(°/s) (±250°/s default).
// prompts: MPU6050 Pico, MPU-6050 RP2040, IMU Raspberry Pi Pico, accel gyro Pico I2C, read MPU6050 GP4 GP5, inertial sensor Pico, MPU6050 Wire setSDA Pico, Pico MPU6050 serial, RP2040 IMU I2C GP4 GP5
// not-tags: nano33iot, lsm6ds3, lsm6, arduino-lsm6ds3, bno055, adxl345
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;  // default I2C address 0x68 (AD0 pin low)

int16_t ax, ay, az;  // raw accel (divide by 16384.0 → g at ±2g range)
int16_t gx, gy, gz;  // raw gyro  (divide by 131.0 → °/s at ±250°/s range)
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);

  // RP2040: assign I2C pins before Wire.begin() — NOT done by hardware automatically
  Wire.setSDA(4);  // GP4 = SDA
  Wire.setSCL(5);  // GP5 = SCL
  Wire.begin();

  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  // testConnection() reads WHO_AM_I register (0x75) — should return 0x68
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found — check wiring and address");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  // getMotion6 requires exactly 6 int16_t pointers — one per axis
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  Serial.print("ax:"); Serial.print(ax);
  Serial.print(" ay:"); Serial.print(ay);
  Serial.print(" az:"); Serial.print(az);
  Serial.print(" gx:"); Serial.print(gx);
  Serial.print(" gy:"); Serial.print(gy);
  Serial.print(" gz:"); Serial.println(gz);
}
