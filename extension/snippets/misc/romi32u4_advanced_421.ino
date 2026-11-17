// snippet: romi32u4_advanced_421
// tags: romi, pololu, romi32u4, ir, sharp, wall-following, distance, control
// boards: romi32u4
// library: Romi32U4
// description: Follow a wall on the right using a Sharp IR sensor on A2, driving the Romi motors
// prompts: follow, right, sharp, sensor, target, distance
// verified: compile-only
// source: kd-generated (id 421)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;

const int IR_PIN = A2;
const float Kp = 0.8;
const int BASE_SPEED = 60;
const int MIN_DISTANCE = 10;  // 10 cm
const int MAX_DISTANCE = 20;  // 20 cm

int lastError = 0;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 50;  // 20 Hz

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    // Read distance from IR sensor
    int raw = analogRead(IR_PIN);
    float distance = 12343.85 / (raw - 3.0);  // Convert to distance in cm

    // Compute error
    int error = MAX_DISTANCE - distance;

    // Compute P-controller output
    int output = Kp * error;

    // Limit output to base speed range
    output = constrain(output, -BASE_SPEED, BASE_SPEED);

    // Apply output to motors (left motor speed + output, right motor speed - output)
    int leftSpeed = BASE_SPEED + output;
    int rightSpeed = BASE_SPEED - output;

    // Romi motors are only reachable through the Romi32U4 library
    motors.setSpeeds(leftSpeed, rightSpeed);

    // Print distance and control output
    Serial.print("distance: "); Serial.print(distance); Serial.print(" cm, ");
    Serial.print("output: "); Serial.println(output);
  }
}
