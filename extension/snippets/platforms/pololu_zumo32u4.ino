// snippet: pololu_zumo32u4
// tags: zumo, zumo32u4, pololu, robot, motor, encoder, imu, line, buttons
// boards: avr
// library: Zumo32U4
// description: Zumo 32U4 robot starter — motors, encoders, and line sensors
// prompts: Pololu Zumo 32U4, Zumo robot starter, Zumo motors encoders line sensors, sumo robot Pololu, Zumo32U4 library
// exact-include: Zumo32U4.h

#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD lcd;

uint16_t lineSensorValues[5];

void setup() {
  lineSensors.initFiveSensors();
  lcd.clear();
  lcd.print("Press A");
  buttonA.waitForButton();
  lcd.clear();
  lcd.print("Go!");
}

void loop() {
  lineSensors.readCalibrated(lineSensorValues);

  // simple line follower: steer toward darkest sensor
  int err = (int)lineSensorValues[4] - (int)lineSensorValues[0];
  int leftSpeed  = constrain(200 + err / 4, -400, 400);
  int rightSpeed = constrain(200 - err / 4, -400, 400);
  motors.setSpeeds(leftSpeed, rightSpeed);

  long leftEnc  = encoders.getCountsLeft();
  long rightEnc = encoders.getCountsRight();
  lcd.gotoXY(0, 1);
  lcd.print(leftEnc); lcd.print(" "); lcd.print(rightEnc);
}
