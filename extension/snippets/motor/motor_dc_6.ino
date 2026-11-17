// snippet: motor_dc_6
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Motor PID speed controller: serial SETPOINT <rpm>, encoder interrupt on pin 2, PWM output pin 5
// prompts: motor, speed, controller, serial, setpoint, <rpm>
// verified: compile-only
// source: kd-generated (id 6)
// exact-include: Arduino.h

#include <Arduino.h>

const int EN = 5;   // PWM enable (ENA on L298N)
const int IN1 = 7;
const int IN2 = 8;
const int ENC_A = 2;  // interrupt-capable pin

volatile long encoderCount = 0;  // volatile: modified in ISR
float setpoint = 0.0;
float measured = 0.0;
float output = 0.0;

float kp = 0.5, ki = 0.1, integral = 0.0, prevError = 0.0;

void motorForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, constrain(speed, 0, 255));
}

void motorBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, constrain(speed, 0, 255));
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 0);
}

// ISR: keep tiny — just count edges. digitalPinToInterrupt maps the pin
// to the correct hardware interrupt vector at compile time.
void onEncoderA() {
  if (digitalRead(ENC_A) == HIGH) encoderCount++;
  else                             encoderCount--;
}

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(EN, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onEncoderA, RISING);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    // "SETPOINT " is 9 chars, so substring(9) starts at the value
    if (input.startsWith("SETPOINT ")) {
      setpoint = input.substring(9).toFloat();
    }
  }

  unsigned long now = millis();
  if (now - prevMs < 10) return;  // 10ms sample period
  prevMs = now;

  // Safely read the volatile ISR counter: disable interrupts
  // briefly so the value can't change mid-copy.
  noInterrupts();
  long count = encoderCount;
  interrupts();
  encoderCount = 0;  // reset for this cycle's delta

  measured = (float)count * (60.0 / 2000.0);  // counts → RPM (2000 counts/rev)

  float error = setpoint - measured;
  integral += error;
  float derivative = (error - prevError) / 10.0;  // dt = 10ms
  prevError = error;

  // PID output: allow negative (reverse) by widening the constrain range
  output = kp * error + ki * integral + derivative;
  output = constrain(output, -255, 255);

  if (output >= 0) {
    motorForward((int)output);
  } else {
    motorBackward((int)abs(output));
  }
}
