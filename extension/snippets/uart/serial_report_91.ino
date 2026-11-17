// snippet: serial_report_91
// tags: serial, report, uart, mpu6050
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: On serial command IMU, read MPU6050 all 6 axes, reply as ax,ay,az,gx,gy,gz
// prompts: serial, command, mpu6050, declare, int16_t, globally
// verified: compile-only
// source: kd-generated (id 91)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;

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
  static unsigned long t = 0;
  if (millis() - t < 100) return;
  t = millis();

  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "IMU") {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)
      Serial.print("ax:"); Serial.print(ax);
      Serial.print("  ay:"); Serial.print(ay);
      Serial.print("  az:"); Serial.print(az);
      Serial.print("  gx:"); Serial.print(gx);
      Serial.print("  gy:"); Serial.print(gy);
      Serial.print("  gz:"); Serial.println(gz);
    }
  }
}
