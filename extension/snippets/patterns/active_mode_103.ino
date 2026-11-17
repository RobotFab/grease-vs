// snippet: active_mode_103
// tags: active, serial, command, response, mpu6050
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: Active sketch: CALIBRATE samples MPU6050 200 times, computes float offsets, stores in global floats
// prompts: active, sketch, calibrate, samples, mpu6050, times
// verified: compile-only
// source: kd-generated (id 103)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
float offsetAx = 0, offsetAy = 0, offsetAz = 0;
float offsetGx = 0, offsetGy = 0, offsetGz = 0;
const int SAMPLES = 200;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);
  }
  Serial.println("MPU6050 ready — send CALIBRATE");
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "CALIBRATE") {
    calibrateMPU6050();
    Serial.println("DONE");
  } else if (cmd == "PING") {
    Serial.println("PONG");
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}

void calibrateMPU6050() {
  int16_t ax, ay, az, gx, gy, gz;
  float sumAx = 0, sumAy = 0, sumAz = 0;
  float sumGx = 0, sumGy = 0, sumGz = 0;

  for (int i = 0; i < SAMPLES; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)
    sumAx += ax; sumAy += ay; sumAz += az;
    sumGx += gx; sumGy += gy; sumGz += gz;
    delay(10);  // spread samples over time
  }

  offsetAx = sumAx / SAMPLES;
  offsetAy = sumAy / SAMPLES;
  offsetAz = sumAz / SAMPLES;
  offsetGx = sumGx / SAMPLES;
  offsetGy = sumGy / SAMPLES;
  offsetGz = sumGz / SAMPLES;

  Serial.print("Offsets: ax="); Serial.print(offsetAx);
  Serial.print(" ay="); Serial.print(offsetAy);
  Serial.print(" az="); Serial.print(offsetAz);
  Serial.print(" gx="); Serial.print(offsetGx);
  Serial.print(" gy="); Serial.print(offsetGy);
  Serial.print(" gz="); Serial.println(offsetGz);
}
