// snippet: multi_motor_robot_592
// tags: robot, multi-motor, differential-drive, millis, mpu6050
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: MPU6050
// description: Differential drive with gyro heading lock: use MPU-6050 yaw rate (gz) to maintain straight heading (L298N ENA=5,IN1=7,IN2=8,ENB=6,IN3=4,IN4=2)
// prompts: differential, drive, heading, mpu-6050, maintain, straight
// verified: compile-only
// source: kd-generated (id 592)
// exact-include: Wire.h, MPU6050.h

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
const float Kp = 2.0f;
const int BASE_SPEED = 180;
int leftSpeed = BASE_SPEED, rightSpeed = BASE_SPEED;

// L298N motor-driver pins (enables are PWM, direction pins are plain digital)
const int ENA = 5, IN1 = 7, IN2 = 8;
const int ENB = 6, IN3 = 4, IN4 = 2;

unsigned long prevMs = 0;
const unsigned long INTERVAL = 20;  // 50 Hz loop

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);
  }
  Serial.println("MPU6050 ready");
}

// Drive one L298N channel: sign of speed selects direction, magnitude sets PWM.
void driveMotor(int en, int inA, int inB, int speed) {
  digitalWrite(inA, speed >= 0 ? HIGH : LOW);
  digitalWrite(inB, speed >= 0 ? LOW : HIGH);
  analogWrite(en, constrain(abs(speed), 0, 255));
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  // Yaw (rotation about the vertical axis) on a flat-mounted MPU-6050 is the Z gyro.
  // Raw counts → deg/s: divide by 131.0 LSB/(deg/s) at the ±250 dps default full scale.
  float yawRate = gz / 131.0f;  // deg/s

  int correction = (int)(Kp * yawRate);

  leftSpeed = BASE_SPEED - correction;
  rightSpeed = BASE_SPEED + correction;

  Serial.print("yawRate: "); Serial.print(yawRate, 2);
  Serial.print(" correction: "); Serial.print(correction);
  Serial.print(" leftSpeed: "); Serial.print(leftSpeed);
  Serial.print(" rightSpeed: "); Serial.println(rightSpeed);

  // Apply the corrected speeds to the two L298N channels.
  driveMotor(ENA, IN1, IN2, leftSpeed);
  driveMotor(ENB, IN3, IN4, rightSpeed);
}
