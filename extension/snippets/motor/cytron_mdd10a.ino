// snippet: cytron_mdd10a
// tags: motor, cytron, mdd10a, mdd3a, dc, driver, pwm, dir
// boards: avr, esp32, megaavr, nano33iot, rp2040
// verified: compile-only
// library: CytronMotorDriver
// description: Dual DC motor with Cytron MDD10A/MDD3A using CytronMotorDriver library
// prompts: Cytron MDD10A motor driver, dual DC motor Cytron, CytronMotorDriver library, MDD3A motor control
// exact-include: CytronMotorDriver.h

#include <CytronMotorDriver.h>

// PWM-DIR mode: Motor1 PWM=3, DIR=4 / Motor2 PWM=5, DIR=6
CytronMD motor1(PWM_DIR, 3, 4);
CytronMD motor2(PWM_DIR, 5, 6);

void setup() {}

void loop() {
  motor1.setSpeed(200);   // 0-255 forward
  motor2.setSpeed(200);
  delay(2000);
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  delay(500);
  motor1.setSpeed(-200);  // reverse
  motor2.setSpeed(-200);
  delay(2000);
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  delay(500);
}
