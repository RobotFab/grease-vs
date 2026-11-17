// snippet: active_mode_233
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: SET_GAIN <kp> <ki> <kd> updates PID gains at runtime
// prompts: active, sketch, set_gain, updates, gains, runtime
// verified: compile-only
// source: kd-generated (id 233)
// exact-include: Arduino.h

#include <Arduino.h>

// PID controller variables
float kp = 1.0, ki = 0.01, kd = 0.1;
float integral = 0.0, prevError = 0.0;
float setpoint = 100.0;
float measured = 0.0;
float output = 0.0;

// Function to update PID gains
void updateGains(float newKp, float newKi, float newKd) {
  kp = newKp;
  ki = newKi;
  kd = newKd;
}

// Function to update setpoint
void updateSetpoint(float newSetpoint) {
  setpoint = newSetpoint;
}

// Function to calculate PID output
float pidUpdate(float sp, float pv, float dt) {
  float err = sp - pv;
  integral += err * dt;
  integral = constrain(integral, -200.0, 200.0);  // anti-windup
  float deriv = (err - prevError) / dt;
  prevError = err;
  return kp * err + ki * integral + kd * deriv;
}

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);  // Wait for USB serial on Leonardo/Nano 33
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 10) {  // 100Hz update rate
    prevMs = now;
    measured = analogRead(A0) * (5.0 / 1023.0) * 100.0;
    output = pidUpdate(setpoint, measured, 0.01);
    output = constrain(output, 0, 255);
    analogWrite(9, (int)output);

    // Handle incoming serial commands
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      if (command.startsWith("SET_GAIN")) {
        float newKp, newKi, newKd;
        sscanf(command.c_str(), "SET_GAIN %f %f %f", &newKp, &newKi, &newKd);
        updateGains(newKp, newKi, newKd);
      } else if (command.startsWith("SETPOINT")) {
        float newSetpoint;
        sscanf(command.c_str(), "SETPOINT %f", &newSetpoint);
        updateSetpoint(newSetpoint);
      } else if (command == "REPORT") {
        Serial.print("sp: "); Serial.print(setpoint);
        Serial.print(" pv: "); Serial.print(measured);
        Serial.print(" out: "); Serial.println(output);
      }
    }
  }
}
