// snippet: active_mode_106
// tags: active, serial, command, response, mpu6050
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: Active sketch: START <hz> begins streaming MPU6050 ax/ay/az
// prompts: active, sketch, start, begins, streaming, mpu6050
// verified: compile-only
// source: kd-generated (id 106)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
bool streaming = false;
unsigned long streamPrev = 0;
long streamInterval = 0;

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
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("START")) {
      // Parse Hz value after "START" + space
      int spaceIdx = command.indexOf(' ');
      int hz = 0;
      if (spaceIdx > 0) {
        hz = command.substring(spaceIdx + 1).toInt();
      }
      if (hz > 0) {
        streamInterval = 1000 / hz;
        streaming = true;
      } else {
        Serial.println("ERR: invalid hz");
      }
    } else if (command == "STOP") {
      streaming = false;
    }
  }

  if (streaming) {
    unsigned long now = millis();
    if (now - streamPrev >= streamInterval) {
      streamPrev = now;
      int16_t ax, ay, az, gx, gy, gz;
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

      Serial.print("AX:");
      Serial.print(ax);
      Serial.print(" AY:");
      Serial.print(ay);
      Serial.print(" AZ:");
      Serial.println(az);
    }
  }
}
