// snippet: ik_3dof_serial
// tags: kinematics, ik, inverse-kinematics, 3dof, serial, arm, servo, robot, xyz, wrist
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 3-DOF serial arm IK (base rotation + 2-DOF planar reach) — positions end-effector in 3D
// prompts: 3DOF serial arm IK, robot arm XYZ position, base elbow wrist IK, 3-axis arm inverse kinematics
// Set L1, L2 to your forearm/upper-arm lengths in mm.
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE: replace with your actual link lengths ----
const float L1 = 130.0f;  // upper arm (mm)
const float L2 = 110.0f;  // forearm  (mm)
// ----------------------------------------------------------

Servo base;    // rotates around Z axis
Servo shoulder;
Servo elbow;

struct Angles { float base, shoulder, elbow; bool ok; };

Angles ik3dof(float x, float y, float z) {
  Angles a;
  a.base = atan2(y, x) * 180.0f / PI;  // azimuth

  float r  = sqrt(x * x + y * y);      // horizontal reach
  float r2 = r * r + z * z;
  float cosE = (r2 - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);
  if (cosE < -1.0f || cosE > 1.0f) { a.ok = false; return a; }

  float elbow_rad    = acos(cosE);
  float shoulder_rad = atan2(z, r) - atan2(L2 * sin(elbow_rad), L1 + L2 * cos(elbow_rad));

  a.shoulder = shoulder_rad * 180.0f / PI;
  a.elbow    = elbow_rad    * 180.0f / PI;
  a.ok = true;
  return a;
}

void setup() {
  Serial.begin(9600);
  base.attach(8); shoulder.attach(9); elbow.attach(10);
}

void loop() {
  // circle in XZ plane at radius 150mm, height 80mm
  for (int deg = 0; deg <= 360; deg += 10) {
    float rad = deg * PI / 180.0f;
    float tx = 150.0f * cos(rad);
    float ty = 0.0f;
    float tz = 80.0f + 40.0f * sin(rad);

    Angles a = ik3dof(tx, ty, tz);
    if (a.ok) {
      base.write(constrain((int)(a.base + 90), 0, 180));
      shoulder.write(constrain((int)(a.shoulder + 90), 0, 180));
      elbow.write(constrain((int)a.elbow, 0, 180));
    }
    delay(80);
  }
}
