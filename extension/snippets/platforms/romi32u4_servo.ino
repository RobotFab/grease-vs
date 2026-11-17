// snippet: romi32u4_servo
// tags: romi, romi32u4, pololu, servo, pwm, joint, arm, pan, tilt, sweep, angle, pca9685, i2c
// boards: romi32u4
// library: Romi32U4, Adafruit PWM Servo Driver Library
// description: Romi 32U4 servo control via PCA9685 I2C servo driver.
//   WHY PCA9685 instead of Servo.h:
//   Servo.h on AVR captures Timer1 for PWM — the same timer Romi32U4Motors uses for drive PWM.
//   Running both causes erratic motor speed and servo jitter. PCA9685 generates PWM in hardware
//   over I2C (SDA=pin 2, SCL=pin 3 on Romi), so no timers are consumed on the 32U4.
//   PCA9685 I2C address: 0x40 (default, all address pins low). Up to 62 boards chained (0x40–0x7F).
//   setPWMFreq(50): standard hobby servo frequency — 50 Hz = 20 ms period.
//   writeMicroseconds(channel, us): sends pulse width in microseconds directly — preferred over
//     setPWM(ch, on, off) raw tick math. 1000 µs = 0°, 1500 µs = 90°, 2000 µs = 180°.
//   NOT: myServo.attach(9) + Romi32U4Motors — Timer1 conflict, do not use together.
//   NOT: Adafruit_PWMServoDriver::setPWM(ch, 0, tick) with arbitrary ticks without knowing
//     the 12-bit resolution: at 50 Hz, 1 tick = 4096/20000 µs ≈ 4.88 µs.
//   Serial: 'ANGLE <ch> <deg>' moves channel ch to deg (0–180). 'READ <ch>' prints last angle.
// prompts: Romi servo PCA9685, Romi32U4 I2C servo, Romi arm servo, Romi pan tilt PCA9685, Romi joint servo, servo on Romi, Romi32U4 servo no timer conflict, Romi servo driver, Romi PCA9685 I2C, Romi servo arm angle
// not-tags: esp32, nano33iot, pico
// exact-include: Romi32U4.h, Wire.h, Adafruit_PWMServoDriver.h

#include <Romi32U4.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // Adafruit PWM Servo Driver Library — NOT Adafruit_Servo

// PCA9685 at default I2C address 0x40
// Romi32U4 I2C bus: SDA=pin2, SCL=pin3 — shared with the onboard LSM6DS33 IMU + LIS3MDL
// magnetometer (encoders are quadrature GPIO, not I2C; there is no onboard I2C display).
// PCA9685 defaults to 0x40, so it does not collide with the IMU (0x6B) or mag (0x1E).
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Pulse width limits in microseconds — calibrate per servo brand
const uint16_t SERVO_MIN_US = 1000;  // 1000 µs = 0°
const uint16_t SERVO_MAX_US = 2000;  // 2000 µs = 180°
const uint8_t  NUM_CHANNELS  = 16;   // PCA9685 has 16 independent channels

int lastAngle[NUM_CHANNELS];  // track last commanded angle per channel

// Convert degrees to microseconds and write to PCA9685 channel
void setServoAngle(uint8_t ch, int degrees) {
  degrees = constrain(degrees, 0, 180);
  uint16_t us = map(degrees, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  pwm.writeMicroseconds(ch, us);  // writeMicroseconds handles the tick math internally
  lastAngle[ch] = degrees;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pwm.begin();
  // 50 Hz = standard hobby servo PWM frequency (20 ms period)
  // Do NOT set above ~330 Hz for analog servos — they will overheat or stall
  pwm.setPWMFreq(50);  // set PWM frequency in Hz (50 = standard hobby servo)

  // Center all channels on startup
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    setServoAngle(i, 90);
    lastAngle[i] = 90;
  }
  Serial.println("PCA9685 servo ready. Commands: ANGLE <ch> <deg> | READ <ch>");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("ANGLE ")) {
      // Format: ANGLE <channel 0-15> <angle 0-180>
      int sp = cmd.indexOf(' ', 6);
      if (sp > 0) {
        uint8_t ch  = (uint8_t)cmd.substring(6, sp).toInt();
        int     deg = cmd.substring(sp + 1).toInt();
        if (ch < NUM_CHANNELS) {
          setServoAngle(ch, deg);
          Serial.print("ch"); Serial.print(ch);
          Serial.print(" -> "); Serial.print(lastAngle[ch]); Serial.println(" deg");
        }
      }
    } else if (cmd.startsWith("READ ")) {
      uint8_t ch = (uint8_t)cmd.substring(5).toInt();
      if (ch < NUM_CHANNELS) {
        Serial.print("ch"); Serial.print(ch);
        Serial.print(": "); Serial.println(lastAngle[ch]);
      }
    }
  }
}
