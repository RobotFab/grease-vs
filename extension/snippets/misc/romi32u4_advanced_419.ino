// snippet: romi32u4_advanced_419
// tags: romi, pololu, romi32u4, encoder, odometry, heading, serial, pose
// boards: romi32u4
// library: Romi32U4
// description: Wheel odometry on Romi 32U4. Reads both wheel encoders via Romi32U4Encoders,
//   integrates x/y/heading every 100 ms, prints to Serial. Button A (Romi32U4ButtonA)
//   resets the pose to the origin. Encoders/buttons MUST use the Romi32U4 library — raw
//   attachInterrupt()/digitalRead() on guessed pins will not see the Romi hardware.
// prompts: compute, wheel, odometry, heading, degrees, encoder
// exact-include: Romi32U4.h, math.h

#include <Romi32U4.h>
#include <math.h>

Romi32U4Encoders encoders;
Romi32U4ButtonA buttonA;

// Physical constants (Pololu Romi 32U4: 70 mm wheels, 141 mm track, 1440 CPR encoders)
const float WHEEL_DIAM_MM = 70.0f;
const float TRACK_MM      = 141.0f;
const int   CPR           = 1440;
const float MM_PER_COUNT  = (float)M_PI * WHEEL_DIAM_MM / CPR;  // ~0.1526 mm/count

// Pose state (mm, radians)
float poseX = 0.0f;
float poseY = 0.0f;
float poseH = 0.0f;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  // Integrate pose at 10 Hz
  if (now - prevMs >= 100) {
    prevMs = now;

    int16_t dL = encoders.getCountsAndResetLeft();
    int16_t dR = encoders.getCountsAndResetRight();

    float distL  = dL * MM_PER_COUNT;
    float distR  = dR * MM_PER_COUNT;
    float dist   = (distL + distR) * 0.5f;
    float dTheta = (distR - distL) / TRACK_MM;

    poseH += dTheta;
    poseX += dist * cos(poseH - dTheta * 0.5f);
    poseY += dist * sin(poseH - dTheta * 0.5f);

    Serial.print("x: ");    Serial.print(poseX, 2);
    Serial.print(" y: ");   Serial.print(poseY, 2);
    Serial.print(" hdg: "); Serial.print(poseH * 180.0f / PI, 2);
    Serial.println(" deg");
  }

  // Button A press -> reset pose to origin
  if (buttonA.getSingleDebouncedPress()) {
    poseX = 0.0f;
    poseY = 0.0f;
    poseH = 0.0f;
    Serial.println("pose reset to origin");
  }
}
