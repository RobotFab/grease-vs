// snippet: robot_arm_ik_474
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Joystick IK: read X from A0 and Y from A1 (each 0–1023, centred at 512)
// prompts: joystick, (each, 0–1023, centred, target, solve
// verified: compile-only
// source: kd-generated (id 474)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE: replace with your actual link lengths ----
const float L1 = 120.0f;  // upper arm length (mm)
const float L2 = 90.0f;   // forearm length (mm)
// ----------------------------------------------------------

Servo shoulder;
Servo elbow;

// Returns true if target is reachable; writes joint angles in degrees to th1, th2
bool ik2dof(float x, float y, float& th1, float& th2) {
  float r2 = x * x + y * y;
  float cosT2 = (r2 - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);
  if (cosT2 < -1.0f || cosT2 > 1.0f) return false;  // out of reach

  float t2 = acos(cosT2);                              // elbow-up solution
  float t1 = atan2(y, x) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));

  th1 = t1 * 180.0f / PI;
  th2 = t2 * 180.0f / PI;
  return true;
}

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 50;  // 20Hz update rate

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    int xRaw = analogRead(A0);
    int yRaw = analogRead(A1);

    // Map joystick values to target coordinates in cm
    float x = map(xRaw, 0, 1023, -100.0f, 100.0f);  // -100cm to 100cm
    float y = map(yRaw, 0, 1023, -100.0f, 100.0f);  // -100cm to 100cm

    float th1, th2;
    if (ik2dof(x, y, th1, th2)) {
      shoulder.write(constrain((int)th1, 0, 180));
      elbow.write(constrain((int)th2, 0, 180));
    } else {
      Serial.println("UNREACHABLE");
    }
  }
}
