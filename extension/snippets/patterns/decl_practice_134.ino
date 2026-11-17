// snippet: decl_practice_134
// tags: globals, declare, millis, nonblocking, mpu6050
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: Declare int16_t ax, ay, az, gx, gy, gz at global scope before calling MPU6050 getMotion6()
// prompts: declare, int16_t, scope, before, calling, mpu6050
// verified: compile-only
// source: kd-generated (id 134)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 100;

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
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

    Serial.print("ax: "); Serial.print(ax);
    Serial.print("  ay: "); Serial.print(ay);
    Serial.print("  az: "); Serial.print(az);
    Serial.print("  gx: "); Serial.print(gx);
    Serial.print("  gy: "); Serial.print(gy);
    Serial.print("  gz: "); Serial.println(gz);
  }
}
