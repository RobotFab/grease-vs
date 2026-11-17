// snippet: combined_128
// tags: combined, multi-sensor, millis, bh1750, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: BH1750, Servo
// description: Light-reactive servo: BH1750 lux over I2C
// prompts: light-reactive, servo, bh1750, declare, bh1750, lightmeter
// verified: compile-only
// source: kd-generated (id 128)
// exact-include: Wire.h, BH1750.h, Servo.h

#include <Wire.h>
#include <BH1750.h>
#include <Servo.h>

BH1750 lightMeter;
float lux = 0.0;
Servo myServo;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  myServo.attach(9);
  myServo.write(90);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  lux = lightMeter.readLightLevel();
  int servoAngle = map(lux, 0.0, 1000.0, 0, 180);
  myServo.write(servoAngle);

  Serial.print("lux: ");
  Serial.print(lux);
  Serial.print("  servo angle: ");
  Serial.println(servoAngle);
}
