// snippet: serial_cmd_87
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: SETPOINT <value> command stores float target
// prompts: setpoint, <value>, command, stores, float, target
// verified: compile-only
// source: kd-generated (id 87)
// exact-include: Arduino.h

#include <Arduino.h>

// Global variables
float setpoint = 0.5;
float kp = 0.8;
float output = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SETPOINT")) {
      setpoint = command.substring(9).toFloat();
      Serial.print("Setpoint set to: "); Serial.println(setpoint);
    }
  }

  // Read the analog input and calculate the error
  float measured = analogRead(A0) / 1023.0;
  float error = setpoint - measured;

  // Calculate the output using the proportional controller.
  // error ranges -1..1, kp=0.8 → output maxes at ~0.8.
  // A raw (int) cast truncates sub-1.0 values to 0, collapsing PWM to 0.
  // Scale to 0-255 so the controller has meaningful actuator authority.
  output = kp * error;
  analogWrite(9, constrain((int)(output * 255), 0, 255));

  // Print the current values for debugging
  Serial.print("Setpoint: "); Serial.print(setpoint);
  Serial.print(" Measured: "); Serial.print(measured);
  Serial.print(" Output: "); Serial.println(output);
}
