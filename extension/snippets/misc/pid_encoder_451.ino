// snippet: pid_encoder_451
// tags: encoder
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: Encoder
// description: Output limiting: clamp PID output to ±200 (analogWrite range)
// prompts: output, limiting, clamp, output, (analogwrite, range)
// verified: compile-only
// source: kd-generated (id 451)
// exact-include: Encoder.h

#include <Encoder.h>

const int ENA = 5, IN1 = 6, IN2 = 7;
const int ENCODER_PIN = 2;

Encoder encoder(ENCODER_PIN, ENCODER_PIN + 1); // Assuming encoder uses pin 2 and 3

int setpoint = 1000; // Desired encoder count
int currentCount = 0;
int error = 0;
int lastError = 0;
int integral = 0;
int derivative = 0;
int output = 0;
const float Kp = 1.0, Ki = 0.1, Kd = 0.01;
const int PWM_MAX = 200;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  currentCount = encoder.read();
  error = setpoint - currentCount;
  derivative = error - lastError;
  integral += error;

  // Clamp integral to prevent windup
  if (integral > 1000) integral = 1000;
  if (integral < -1000) integral = -1000;

  output = Kp * error + Ki * integral + Kd * derivative;

  // Clamp output to PWM range
  if (output > PWM_MAX) output = PWM_MAX;
  if (output < -PWM_MAX) output = -PWM_MAX;

  // Apply PWM to motor
  if (output >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(ENA, abs(output));

  lastError = error;
  Serial.print("Error: "); Serial.println(error);
  Serial.print("Output: "); Serial.println(output);
  delay(10); // Adjust delay as needed
}
