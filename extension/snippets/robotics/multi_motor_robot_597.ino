// snippet: multi_motor_robot_597
// tags: robot, multi-motor, differential-drive, millis
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Robot odometry with encoder feedback: track x, y, heading using differential drive kinematics (wheel encoders on interrupt pins 2/3)
// prompts: robot, odometry, encoder, feedback, track, heading
// verified: compile-only
// source: kd-generated (id 597)
// exact-include: Arduino.h

#include <Arduino.h>

const int LEFT_MOTOR_PIN = 5;
const int RIGHT_MOTOR_PIN = 11;
// Encoders must be on hardware-interrupt-capable pins. On classic AVR (Uno) that is
// only pins 2 and 3, so both encoders live there; all other listed boards support these too.
const int LEFT_ENCODER_PIN = 2;
const int RIGHT_ENCODER_PIN = 3;
const int BUTTON_PIN = 4;

const float WHEEL_RADIUS = 33.0f;  // mm
const float WHEELBASE = 160.0f;   // mm
const float WHEEL_CPR = 360.0f;   // counts per revolution

volatile long leftEncoderCount = 0;
volatile long rightEncoderCount = 0;
float x = 0.0f;
float y = 0.0f;
float heading = 0.0f;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  pinMode(LEFT_ENCODER_PIN, INPUT);
  pinMode(RIGHT_ENCODER_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN), countLeftEncoder, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN), countRightEncoder, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 50) {  // 20 Hz update rate
    prevMs = now;

    noInterrupts();
    long leftCount = leftEncoderCount;
    long rightCount = rightEncoderCount;
    leftEncoderCount = 0;
    rightEncoderCount = 0;
    interrupts();

    // Arc length each wheel rolled this tick, in mm. Keep everything in mm so it is
    // dimensionally consistent with WHEELBASE (mm) below — dividing distance by 1000
    // (to metres) while WHEELBASE stays in mm makes the heading update 1000x too small.
    float leftWheelDist  = 2 * PI * WHEEL_RADIUS * (leftCount  / WHEEL_CPR);
    float rightWheelDist = 2 * PI * WHEEL_RADIUS * (rightCount / WHEEL_CPR);

    float deltaHeading = (rightWheelDist - leftWheelDist) / WHEELBASE;  // rad = mm / mm
    heading += deltaHeading;

    float deltaDistance = (rightWheelDist + leftWheelDist) / 2.0f;  // mm
    x += deltaDistance * cos(heading);
    y += deltaDistance * sin(heading);

    Serial.print("x: "); Serial.print(x, 2);
    Serial.print(" y: "); Serial.print(y, 2);
    Serial.print(" heading: "); Serial.println(heading * 180.0f / PI, 2);

  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    x = 0.0f;
    y = 0.0f;
    heading = 0.0f;
    Serial.println("Pose reset");
  }
}

void countLeftEncoder() {
  leftEncoderCount++;
}

void countRightEncoder() {
  rightEncoderCount++;
}
