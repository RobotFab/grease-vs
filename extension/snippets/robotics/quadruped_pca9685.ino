// snippet: quadruped_pca9685
// tags: quadruped, robot, servo, pca9685, i2c, 12-servo, esp32, init, channel, coxa, femur, tibia, leg, mapping
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: Adafruit PWM Servo Driver Library
// description: ESP32 quadruped — 12-servo init via PCA9685 I2C servo driver with per-leg channel mapping.
//   PCA9685 is the correct approach for 12+ servos on ESP32 — do not use Servo.h for 12 channels.
//   ESP32 has limited hardware PWM timers; attempting 12 Servo objects causes conflicts or wrong signals.
//   PCA9685 generates all PWM in hardware over I2C (SDA/SCL) — zero timer consumption on ESP32.
//   I2C address: 0x40 (default). Chain up to 62 boards for 992 total channels.
//   writeMicroseconds(ch, us): preferred over raw setPWM ticks. 1000 µs=0°, 1500 µs=90°, 2000 µs=180°.
//   NOT: Servo myServo[12]; myServo[i].attach(pin) — ESP32 timer conflicts with 12 instances.
//   NOT: analogWrite(pin, val) — ESP32 analogWrite drives LEDC, not servo-compatible PWM.
//   Power the servos from their OWN supply (5-6 V, several amps) and tie only the grounds
//   together. Twelve servos stalling at once pull far more than any board's regulator, and the
//   brownout resets the MCU mid-gait — which drops the robot.
//   The pulse limits below (SERVO_MIN_US/SERVO_MAX_US) are what map degrees to microseconds:
//   with 500/2500 it is 500 us = 0 deg, 1500 us = 90 deg, 2500 us = 180 deg.
//   Many servos want 1000/2000 instead — driving past a servo's real end stop makes it buzz and
//   overheat. Calibrate one joint before energising all twelve.
//   homeAll() moves every joint at once with no ramp, so at power-on the legs SNAP to the home
//   pose. Do the first power-up with the body on a stand.
//   Channel layout (standard 3-DOF quadruped):
//     FL: coxa=0, femur=1,  tibia=2
//     FR: coxa=3, femur=4,  tibia=5
//     BL: coxa=6, femur=7,  tibia=8
//     BR: coxa=9, femur=10, tibia=11
// prompts: quadruped PCA9685, 12 servo quadruped ESP32, ESP32 quadruped servo driver, quadruped servo init PCA9685, 12-DOF quadruped ESP32, quadruped coxa femur tibia PCA9685, quadruped I2C servo driver, ESP32 12 servo robot, quadruped servo channel mapping, PCA9685 quadruped setup, initialise quadruped servos, quadruped servo assign channels
// not-tags: servo.h, romi32u4, romi
// exact-include: Wire.h, Adafruit_PWMServoDriver.h

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // Adafruit PWM Servo Driver Library — NOT Adafruit_Servo

// PCA9685 at default I2C address 0x40 — SDA=GPIO21, SCL=GPIO22 on ESP32 DevKit
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Servo pulse limits — calibrate per servo model
const uint16_t SERVO_MIN_US = 500;   // 500 µs ≈ 0°   (some servos need 500 not 1000)
const uint16_t SERVO_MAX_US = 2500;  // 2500 µs ≈ 180°

// Channel map: [leg][joint] — leg: 0=FL,1=FR,2=BL,3=BR | joint: 0=coxa,1=femur,2=tibia
const uint8_t CH[4][3] = {
  {0, 1, 2},   // FL
  {3, 4, 5},   // FR
  {6, 7, 8},   // BL
  {9, 10, 11}, // BR
};

// Write degrees to a specific leg joint via PCA9685
void setJoint(uint8_t leg, uint8_t joint, int degrees) {
  degrees = constrain(degrees, 0, 180);
  uint16_t us = map(degrees, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  // writeMicroseconds(channel, pulse_us) — the limits above define the mapping:
  // 500 us = 0 deg, 1500 us = 90 deg, 2500 us = 180 deg.
  pwm.writeMicroseconds(CH[leg][joint], us);
}

// Home pose: coxa=90°(neutral), femur=45°(forward-lean), tibia=90°(vertical)
void homeAll() {
  for (uint8_t leg = 0; leg < 4; leg++) {
    setJoint(leg, 0, 90);  // coxa neutral
    setJoint(leg, 1, 45);  // femur
    setJoint(leg, 2, 90);  // tibia
  }
}

void setup() {
  Serial.begin(115200);
  // Default I2C pins: ESP32 SDA=21/SCL=22, Uno/Nano A4/A5, Mega 20/21, Nano 33 IoT 11/12.
  Wire.begin();

  if (!pwm.begin()) {
    Serial.println("PCA9685 not found — check SDA/SCL and the address jumpers");
    while (1);   // halt: `return` here would let loop() run against a driver that never woke up
  }
  pwm.setPWMFreq(50);     // 50 Hz = standard hobby servo frequency (20 ms period)

  homeAll();
  Serial.println("Quadruped ready");
}

void loop() {
  // Replace with gait logic, IK calls, or serial command parser
}
