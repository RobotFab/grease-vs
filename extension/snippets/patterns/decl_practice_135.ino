// snippet: decl_practice_135
// tags: globals, declare, millis, nonblocking, mpu6050
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: Declare float roll=0
// prompts: declare, float, roll=0, pitch=0, yaw=0, scope
// verified: compile-only
// source: kd-generated (id 135)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;

float roll = 0.0, pitch = 0.0, yaw = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
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
  if (now - prevMs < 20) return;
  prevMs = now;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  // Complementary filter parameters
  const float alpha = 0.98f;  // Adjust this value for your application

  // Convert raw gyroscope values to radians per second
  float gxRad = gx * (PI / 180.0);
  float gyRad = gy * (PI / 180.0);
  float gzRad = gz * (PI / 180.0);

  // Update roll and pitch from accelerometer
  float accelRoll = atan2(ay, az);
  float accelPitch = atan2(-ax, sqrt(ay * ay + az * az));

  // Update yaw from gyroscope
  yaw += gzRad * 0.02f;  // dt = 20ms

  // Complementary filter
  roll = alpha * (roll + gxRad * 0.02f) + (1.0f - alpha) * accelRoll;
  pitch = alpha * (pitch + gyRad * 0.02f) + (1.0f - alpha) * accelPitch;

  Serial.print("roll: "); Serial.print(roll * 180.0 / PI, 2);
  Serial.print("  pitch: "); Serial.print(pitch * 180.0 / PI, 2);
  Serial.print("  yaw: "); Serial.println(yaw * 180.0 / PI, 2);
}
