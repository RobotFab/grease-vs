// snippet: esp32_imu_160
// tags: esp32, imu, mpu6050, i2c
// boards: esp32
// library: MPU6050
// description: ESP32: read MPU6050 over I2C (SDA=21, SCL=22)
// prompts: esp32, mpu6050, (sda=21, scl=22), declare, int16_t
// verified: compile-only
// source: kd-generated (id 160)
// exact-include: Wire.h, MPU6050.h
// NOTE: Wire.begin(21, 22) is the ESP32-only two-arg pin-select overload.
//   SAMD21 (nano33iot) Wire.begin() takes no arguments.

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22);
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);
  }
  Serial.println("MPU6050 ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 20) {  // 50Hz
    prevMs = now;
    mpu.getMotion6(&ax, &ay, &az, NULL, NULL, NULL);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

    Serial.print(ax); Serial.print(",");
    Serial.print(ay); Serial.print(",");
    Serial.println(az);
  }
}
