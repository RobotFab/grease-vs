// snippet: robot_arm_fk_457
// tags: robot, arm, fk, forward-kinematics, servo
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 3-DOF planar FK: read θ1,θ2,θ3 from A0,A1,A2
// prompts: 3-dof, planar, compute, end-effector, where, φ=θ1+θ2+θ3
// verified: compile-only
// source: kd-generated (id 457)
// exact-include: math.h

#include <math.h>

// ---- CONFIGURE ----
const float L1 = 12.0f;  // upper arm length (cm)
const float L2 = 9.0f;   // forearm length (cm)
const float L3 = 6.0f;   // wrist length (cm)
// -------------------

struct Vec3 { float x, y, z; };

// th0 = base azimuth, th1 = shoulder elevation, th2 = elbow angle (all degrees)
Vec3 forwardKinematics(float th0, float th1, float th2) {
  float t0 = th0 * PI / 180.0f;
  float t1 = th1 * PI / 180.0f;
  float t2 = th2 * PI / 180.0f;

  float r  = L2 * cos(t1) + L3 * cos(t1 + t2);
  Vec3 pos;
  pos.x = r * cos(t0);
  pos.y = r * sin(t0);
  pos.z = L1 + L2 * sin(t1) + L3 * sin(t1 + t2);
  return pos;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 100;  // 10 Hz

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    float th1 = analogRead(A0) * 360.0f / 1023.0f;  // Convert A0 reading to degrees
    float th2 = analogRead(A1) * 360.0f / 1023.0f;  // Convert A1 reading to degrees
    float th3 = analogRead(A2) * 360.0f / 1023.0f;  // Convert A2 reading to degrees

    Vec3 pos = forwardKinematics(th1, th2, th3);
    float phi = th1 + th2 + th3;

    Serial.print("th1: "); Serial.print(th1, 2);
    Serial.print("  th2: "); Serial.print(th2, 2);
    Serial.print("  th3: "); Serial.print(th3, 2);
    Serial.print("  x: "); Serial.print(pos.x, 2);
    Serial.print("  y: "); Serial.print(pos.y, 2);
    Serial.print("  phi: "); Serial.println(phi, 2);
  }
}
