// snippet: nano33iot_advanced_574
// tags: arduino_lsm6ds3, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Arduino_LSM6DS3, Servo
// description: IMU-controlled servo: map LSM6DS3 pitch angle (-90° to 90°) to servo angle (0° to 180°) using Servo
// prompts: imu-controlled, servo, lsm6ds3, pitch, angle, (-90°
// verified: compile-only
// source: kd-generated (id 574)
// exact-include: Arduino_LSM6DS3.h, Servo.h

#include <Arduino_LSM6DS3.h>
#include <Servo.h>

Servo myServo;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 50;  // 20 Hz update rate

void setup() {
  Serial.begin(9600);
  myServo.attach(9);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    float pitch = atan2(ay, az) * 180.0 / PI;
    // Map pitch (-90..+90) to servo angle (0..180)
    int angle = constrain((int)(pitch + 90.0), 0, 180);
    myServo.write(angle);
    Serial.print("pitch: "); Serial.print(pitch);
    Serial.print(" servo: "); Serial.println(angle);
  }
}
