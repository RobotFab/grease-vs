// snippet: lsm6dsox_i2c
// tags: imu, lsm6dsox, accelerometer, gyroscope, i2c, wire, external, 6dof
// boards: avr, esp32, megaavr, nano33iot, rp2040, nano33ble
// library: Arduino_LSM6DSOX
// description: External LSM6DSOX breakout wired over I2C — works on any board with Wire.
//   Default I2C address is 0x6B (SA0 high) or 0x6A (SA0 to GND).
//   Wiring: SDA → board SDA, SCL → board SCL, VCC → 3.3V, GND → GND.
//   Uses the official Arduino_LSM6DSOX library with a custom-address constructor.
// prompts: LSM6DSOX external I2C, 6DOF IMU any board, external IMU over I2C Wire, LSM6DSOX breakout, accel gyro I2C
// not-tags: mpu6050, mpu9250, lsm6ds3, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Wire.h, Arduino_LSM6DSOX.h

#include <Wire.h>
#include <Arduino_LSM6DSOX.h>

// ---- CONFIGURE ----
#define LSM_ADDR  0x6B   // change to 0x6A if SA0 pin is pulled LOW
// -------------------

// Own instance bound to Wire at the breakout's address (default IMU uses a fixed address).
LSM6DSOXClass imu(Wire, LSM_ADDR);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!imu.begin()) {
    Serial.println("LSM6DSOX not found — check wiring and address");
    while (1);
  }
  Serial.println("LSM6DSOX ready");
}

void loop() {
  float ax, ay, az, gx, gy, gz;
  if (imu.accelerationAvailable() && imu.gyroscopeAvailable()) {
    imu.readAcceleration(ax, ay, az);   // g
    imu.readGyroscope(gx, gy, gz);      // dps
    Serial.print("A: "); Serial.print(ax, 3); Serial.print(" ");
    Serial.print(ay, 3); Serial.print(" "); Serial.println(az, 3);
    Serial.print("G: "); Serial.print(gx, 2); Serial.print(" ");
    Serial.print(gy, 2); Serial.print(" "); Serial.println(gz, 2);
  }
  delay(20);
}
