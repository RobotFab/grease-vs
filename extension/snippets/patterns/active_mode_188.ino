// snippet: active_mode_188
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: PID <kp> <ki> <kd> sets PID gains at runtime
// prompts: active, sketch, gains, runtime, setpoint, <val>
// verified: compile-only
// source: kd-generated (id 188)
// exact-include: Arduino.h

#include <Arduino.h>

const int INPUT_PIN = A0;
const int OUTPUT_PIN = 5;

float kp = 1.0f;
float ki = 0.01f;
float kd = 0.1f;
float setpoint = 512.0f;  // target ADC value (0-1023)
float measured = 0.0f;
float output = 0.0f;

float integral = 0.0f;
float prevError = 0.0f;
unsigned long prevPidMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  prevPidMs = millis();
  Serial.println("Send: PID <kp> <ki> <kd>  or  SET <val>");
}

void loop() {
  unsigned long now = millis();

  // Handle serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("PID")) {
      // Parse: PID <kp> <ki> <kd>
      int s1 = command.indexOf(' ');
      int s2 = command.indexOf(' ', s1 + 1);
      int s3 = command.indexOf(' ', s2 + 1);
      if (s1 > 0 && s2 > 0 && s3 > 0) {
        kp = command.substring(s1 + 1, s2).toFloat();
        ki = command.substring(s2 + 1, s3).toFloat();
        kd = command.substring(s3 + 1).toFloat();
        integral = 0.0f;  // reset integral on gain change
        prevError = 0.0f;
        Serial.println("Gains updated");
      }
    } else if (command.startsWith("SET")) {
      int s1 = command.indexOf(' ');
      if (s1 > 0) {
        setpoint = command.substring(s1 + 1).toFloat();
      }
    }
  }

  // PID loop — use actual dt, not the print timer
  float dt = (now - prevPidMs) / 1000.0f;  // seconds
  if (dt > 0.0f) {
    prevPidMs = now;

    measured = (float)analogRead(INPUT_PIN);
    float error = setpoint - measured;

    integral += error * dt;
    integral = constrain(integral, -200.0f, 200.0f);  // anti-windup

    float derivative = (error - prevError) / dt;
    prevError = error;

    output = kp * error + ki * integral + kd * derivative;
    output = constrain(output, 0.0f, 255.0f);

    analogWrite(OUTPUT_PIN, (int)output);
  }

  // Report every 100ms
  static unsigned long lastPrintMs = 0;
  if (now - lastPrintMs >= 100) {
    lastPrintMs = now;
    Serial.print("Error: ");
    Serial.println(setpoint - measured);
    Serial.print("Output: ");
    Serial.println((int)output);
  }
}
