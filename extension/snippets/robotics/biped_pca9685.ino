// snippet: biped_pca9685
// tags: biped, bipedal, robot, servo, pca9685, i2c, 6-servo, esp32, hip, knee, ankle, leg, walking, balance, pose
// boards: esp32
// library: Adafruit PWM Servo Driver Library
// description: ESP32 6-servo biped via PCA9685 — channel mapping, pose table, and blocking pose interpolation.
//   6 servos: L_hip=0, L_knee=1, L_ankle=2, R_hip=3, R_knee=4, R_ankle=5.
//   PCA9685 avoids ESP32 timer conflicts that arise with 6 Servo.h instances.
//   writeMicroseconds(ch, us): 1000 µs=0°, 1500 µs=90°, 2000 µs=180°.
//   Interpolation: always move joints gradually — jumping to a pose instantly tips the robot.
//   Weight shift before swing: shift CoM over stance leg before lifting swing leg.
//   NOT: Servo myServo[6]; on ESP32 — timer conflicts degrade PWM accuracy beyond 4 instances.
// prompts: biped PCA9685, 6 servo biped ESP32, bipedal robot servo driver, biped hip knee ankle PCA9685, bipedal ESP32 servo init, biped pose table, bipedal walk ESP32, biped servo channel map, bipedal robot setup, 6-DOF biped ESP32, biped balance pose, bipedal interpolation, biped stand walk sit ESP32
// not-tags: arduino:avr:uno, arduino:avr:mega, Servo.h
// exact-include: Wire.h, Adafruit_PWMServoDriver.h

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

const uint16_t SERVO_MIN_US = 500;
const uint16_t SERVO_MAX_US = 2500;

// Joint indices (PCA9685 channels)
enum Joint { L_HIP=0, L_KNEE=1, L_ANKLE=2, R_HIP=3, R_KNEE=4, R_ANKLE=5 };
const uint8_t NUM_JOINTS = 6;

// Pose table [joint] in degrees
const int POSE_STAND[NUM_JOINTS]  = {90, 90, 90, 90, 90, 90};
const int POSE_SIT[NUM_JOINTS]    = {90, 45, 90, 90, 45, 90};  // knees bent
const int POSE_LEAN_R[NUM_JOINTS] = {90, 90, 80, 90, 90, 100}; // shift CoM right (ankles tilt)
const int POSE_LEAN_L[NUM_JOINTS] = {90, 90, 100, 90, 90, 80}; // shift CoM left

void setJoint(uint8_t ch, int degrees) {
  degrees = constrain(degrees, 0, 180);
  pwm.writeMicroseconds(ch, map(degrees, 0, 180, SERVO_MIN_US, SERVO_MAX_US));  // 1000us=0deg, 1500us=90deg, 2000us=180deg (pulse width, channel, us)
}

// Interpolate from current pose to target pose over durationMs
// currentPose[] is updated in place so next call starts from correct position
void moveToPose(int* currentPose, const int* targetPose, unsigned int durationMs) {
  const int STEPS = 50;
  unsigned int stepMs = durationMs / STEPS;
  for (int s = 1; s <= STEPS; s++) {
    for (uint8_t j = 0; j < NUM_JOINTS; j++) {
      // Linear interpolation: current + fraction * (target - current)
      int angle = currentPose[j] + (targetPose[j] - currentPose[j]) * s / STEPS;
      setJoint(j, angle);
    }
    delay(stepMs);
  }
  // Sync current pose
  for (uint8_t j = 0; j < NUM_JOINTS; j++) currentPose[j] = targetPose[j];
}

int pose[NUM_JOINTS];  // tracks current joint angles

void setup() {
  Serial.begin(115200);
  Wire.begin();         // ESP32 default I2C: SDA=21, SCL=22
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz standard servo frequency

  // Initialise to stand pose
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    pose[j] = POSE_STAND[j];
    setJoint(j, pose[j]);
  }
  delay(1000);
  Serial.println("Biped ready");
}

void loop() {
  // Example: sit → lean right → stand sequence
  moveToPose(pose, POSE_SIT,    800);
  delay(500);
  moveToPose(pose, POSE_LEAN_R, 400);  // shift CoM before lifting left leg
  delay(300);
  moveToPose(pose, POSE_STAND,  800);
  delay(1000);
}
