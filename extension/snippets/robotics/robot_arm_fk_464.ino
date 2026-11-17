// snippet: robot_arm_fk_464
// tags: robot, arm, fk, forward-kinematics, servo
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 2-DOF Jacobian: compute end-effector velocity [vx, vy] from joint velocities [dθ1, dθ2] using the…
// prompts: 2-dof, jacobian, compute, end-effector, velocity, joint
// verified: compile-only
// source: kd-generated (id 464)
// exact-include: math.h

#include <math.h>

const float L1 = 120.0f;  // upper arm length (mm)
const float L2 = 100.0f;  // forearm length (mm)

struct Vec2 { float x, y; };

// 2x2 Jacobian: rows are [vx; vy], columns are the partials w.r.t. [theta1, theta2].
struct Jac2 {
  float j11, j12;   // dx/dtheta1, dx/dtheta2
  float j21, j22;   // dy/dtheta1, dy/dtheta2
};

Vec2 forwardKinematics(float th1, float th2) {
  float t1 = th1 * PI / 180.0f;
  float t2 = th2 * PI / 180.0f;

  Vec2 pos;
  pos.x = L1 * cos(t1) + L2 * cos(t1 + t2);
  pos.y = L1 * sin(t1) + L2 * sin(t1 + t2);
  return pos;
}

Jac2 jacobian(float th1, float th2) {
  float t1 = th1 * PI / 180.0f;
  float t2 = th2 * PI / 180.0f;

  Jac2 J;
  J.j11 = -L1 * sin(t1) - L2 * sin(t1 + t2);
  J.j12 = -L2 * sin(t1 + t2);
  J.j21 =  L1 * cos(t1) + L2 * cos(t1 + t2);
  J.j22 =  L2 * cos(t1 + t2);
  return J;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 100; // 10 Hz refresh rate

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    static float prevTh1 = 0.0f, prevTh2 = 0.0f;
    float th1 = analogRead(A0) * (180.0 / 1023.0);
    float th2 = analogRead(A1) * (180.0 / 1023.0);

    float dTh1 = (th1 - prevTh1) * (PI / 180.0f);
    float dTh2 = (th2 - prevTh2) * (PI / 180.0f);

    Jac2 J = jacobian(prevTh1, prevTh2);
    Vec2 v = { J.j11 * dTh1 + J.j12 * dTh2,
               J.j21 * dTh1 + J.j22 * dTh2 };

    Serial.print("vx: "); Serial.print(v.x, 2);
    Serial.print("  vy: "); Serial.println(v.y, 2);

    prevTh1 = th1;
    prevTh2 = th2;
  }
}
