// snippet: pico_motor_298
// tags: pico, rp2040, motor
// boards: rp2040
// library: none
// description: RPi Pico: DC motor via L298N on pins 2 (ENA PWM), 3 (IN1), 4 (IN2)
// prompts: motor, l298n, (in1), (in2), declare, motorspeed=0
// verified: compile-only
// source: kd-generated (id 298)
// exact-include: Arduino.h

#include <Arduino.h>

int motorSpeed = 0;
int motorDir = 0;

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT); // IN1
  pinMode(4, OUTPUT); // IN2
  pinMode(2, OUTPUT); // ENA — on RP2040 all GPIO pins support PWM via analogWrite()
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.startsWith("FWD")) {
      motorDir = 1;
      motorSpeed = command.substring(3).toInt();
      motorForward(motorSpeed);
    } else if (command.startsWith("REV")) {
      motorDir = -1;
      motorSpeed = command.substring(3).toInt();
      motorBackward(motorSpeed);
    } else if (command == "STOP") {
      motorStop();
    }
  }
}

void motorForward(int speed) {
  analogWrite(2, speed);
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
}

void motorBackward(int speed) {
  analogWrite(2, speed);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
}

void motorStop() {
  analogWrite(2, 0);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
}
