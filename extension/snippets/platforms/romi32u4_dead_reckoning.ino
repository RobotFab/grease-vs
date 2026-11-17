// snippet: romi32u4_dead_reckoning
// tags: romi, pololu, romi32u4, odometry, dead-reckoning, pose, position, heading, encoder, x, y
// boards: romi32u4
// library: Romi32U4
// description: Dead-reckoning pose estimation on Romi32U4 using encoder counts.
//   Constants: WHEEL_DIAM=70mm TRACK=141mm CPR=1440.
//   Each cycle: read & reset encoder deltas, compute dDist and dTheta, integrate x/y/heading.
//   Prints X Y heading every 500ms. getCountsAndResetLeft/Right resets counts after read.
//   NOT: #include <Romi32U4IMU.h> — no such header. IMU is in the separate LSM6 library.
//   For IMU use: #include <LSM6.h> and LSM6 imu; — see romi32u4_imu_buzzer snippet.
//   NOT: motors.setSpeeds(...) in setup() with no way to stop. A robot that starts driving
//   the instant it is powered on drives off the bench while you are still plugging in the USB
//   cable. Gate motion behind button A and give button B a stop path.
// prompts: Romi dead reckoning, Romi odometry, Romi32U4 pose estimation, Romi track position XY, Romi encoder position heading, Romi X Y heading, Romi odometry dead reckoning encoder
// exact-include: Romi32U4.h, math.h

#include <Romi32U4.h>
#include <math.h>

Romi32U4Encoders encoders;
Romi32U4Motors   motors;
Romi32U4ButtonA  btnA;   // press to start driving
Romi32U4ButtonB  btnB;   // press to stop

// Physical constants
const float WHEEL_DIAM_MM = 70.0f;
const float TRACK_MM      = 141.0f;
const int   CPR           = 1440;
const float MM_PER_COUNT  = (float)M_PI * WHEEL_DIAM_MM / CPR;

// Pose state
float x       = 0.0f;  // mm
float y       = 0.0f;  // mm
float heading = 0.0f;  // radians

unsigned long poseMs   = 0;
unsigned long updateMs = 0;

const int DRIVE_SPEED = 80;   // ±300 (allowTurbo(true) raises the limit to ±400)
bool driving = false;

void setup() {
  Serial.begin(9600);
  // Motors stay stopped until the operator asks for motion.
  motors.setSpeeds(0, 0);
  Serial.println("A = drive, B = stop");
}

void loop() {
  unsigned long now = millis();

  if (btnA.getSingleDebouncedPress()) {
    driving = true;
    motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);
  }
  if (btnB.getSingleDebouncedPress()) {
    driving = false;
    motors.setSpeeds(0, 0);
  }

  // Pose keeps integrating whether or not the wheels are driven — pushing the robot
  // by hand still moves it, and the encoders still see it.

  // Update pose at 50 Hz
  if (now - updateMs >= 20) {
    updateMs = now;

    int16_t dL = encoders.getCountsAndResetLeft();
    int16_t dR = encoders.getCountsAndResetRight();

    float dLeft  = dL * MM_PER_COUNT;
    float dRight = dR * MM_PER_COUNT;
    float dDist  = (dLeft + dRight) * 0.5f;
    float dTheta = (dRight - dLeft) / TRACK_MM;

    heading += dTheta;
    x       += dDist * cos(heading - dTheta * 0.5f);
    y       += dDist * sin(heading - dTheta * 0.5f);
  }

  // Print pose at 2 Hz
  if (now - poseMs >= 500) {
    poseMs = now;
    Serial.print("x="); Serial.print(x, 1);
    Serial.print(" y="); Serial.print(y, 1);
    Serial.print(" hdg="); Serial.println(heading * 180.0f / M_PI, 1);
  }
}
