// snippet: pca9685_multi_servo
// tags: servo, pca9685, adafruit, pwm, i2c, multi, 16-channel, driver
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit PWM Servo Driver Library
// description: Control up to 16 servos from one I2C PCA9685 board
// prompts: PCA9685 multi servo, 16-channel servo driver, many servos I2C, servo multiplexer, Adafruit PWM servo board
// exact-include: Wire.h, Adafruit_PWMServoDriver.h

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Adjust these for your servo
#define SERVO_MIN 150   // pulse count for 0 degrees
#define SERVO_MAX 600   // pulse count for 180 degrees

uint16_t degreeToPulse(int deg) {
  return map(deg, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz for standard servos
  // Set all servos to 90 degrees
  for (int i = 0; i < 16; i++) {
    pwm.setPWM(i, 0, degreeToPulse(90));  // setPWM(channel, on, off): raw 12-bit duty (0..4095)
  }
}

void loop() {
  pwm.setPWM(0, 0, degreeToPulse(0));  // setPWM(channel, on, off): raw 12-bit duty (0..4095)
  delay(1000);
  pwm.setPWM(0, 0, degreeToPulse(180));  // setPWM(channel, on, off): raw 12-bit duty (0..4095)
  delay(1000);
}
