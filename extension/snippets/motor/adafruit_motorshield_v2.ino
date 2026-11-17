// snippet: adafruit_motorshield_v2
// tags: motor, adafruit, motorshield, shield, pca9685, i2c, dc, stepper
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit Motor Shield V2 Library
// description: DC and stepper motor control with Adafruit Motor Shield V2 (I2C PCA9685)
// prompts: Adafruit motor shield v2, PCA9685 I2C motor, DC stepper motor shield, Arduino motor shield, Adafruit motorshield
// exact-include: Wire.h, Adafruit_MotorShield.h

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield shield;
Adafruit_DCMotor *dc1 = shield.getMotor(1);
Adafruit_DCMotor *dc2 = shield.getMotor(2);

void setup() {
  Serial.begin(9600);
  shield.begin();
  dc1->setSpeed(200);
  dc2->setSpeed(200);
}

void loop() {
  dc1->run(FORWARD);
  dc2->run(FORWARD);
  delay(2000);
  dc1->run(RELEASE);
  dc2->run(RELEASE);
  delay(500);
}
