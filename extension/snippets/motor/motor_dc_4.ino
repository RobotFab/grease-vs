// snippet: motor_dc_4
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Ramp DC motor via L298N pins 5/6 from speed 0 to 200 in 50 steps over 3 seconds using millis
// prompts: motor, l298n, speed, steps, seconds, delay()
// verified: compile-only
// source: kd-generated (id 4)
// exact-include: Arduino.h

#include <Arduino.h>

const int ENA = 5;
const int IN1 = 7;
const int IN2 = 8;

void motor(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -speed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  static unsigned long startMs = 0;
  static int step = 0;
  static int targetSpeed = 200;
  static int steps = 50;
  static int stepSize = targetSpeed / steps;

  if (startMs == 0) {
    startMs = millis();
  }

  unsigned long now = millis();
  if (now - startMs >= (step + 1) * (3000 / steps)) {
    step++;
    int currentSpeed = step * stepSize;
    motor(currentSpeed);
    if (step >= steps) {
      motor(0);
      startMs = 0;
      step = 0;
    }
  }
}
