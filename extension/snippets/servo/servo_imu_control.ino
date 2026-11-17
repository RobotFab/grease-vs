// snippet: servo_imu_control
// tags: servo, imu, lsm6ds3, arduino_lsm6ds3, nano33iot, master, slave, tilt, control, angle
// boards: nano33iot
// library: Arduino_LSM6DS3, Servo
// description: Master/slave — control servo angle from LSM6DS3 pitch using readAcceleration()
// prompts: IMU servo control, tilt-controlled servo, accelerometer pitch to servo, LSM6DS3 servo angle, gyro servo tilt
// not-tags: mpu6050, mpu9250, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: Arduino_LSM6DS3.h, Servo.h

#include <Arduino_LSM6DS3.h>
#include <Servo.h>

Servo myServo;
unsigned long prevMs = 0;

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
  if (now - prevMs < 20) return;
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
