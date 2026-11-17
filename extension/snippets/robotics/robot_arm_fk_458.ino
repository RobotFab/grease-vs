// snippet: robot_arm_fk_458
// tags: robot, arm, fk, forward-kinematics, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 2-DOF FK with servo output: θ1 from A0, θ2 from A1 (0–180°)
// prompts: 2-dof, servo, output, (0–180°), compute, simultaneously
// verified: compile-only
// source: kd-generated (id 458)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

Servo shoulder;
Servo elbow;

// ---- CONFIGURE ----
const float L1 = 120.0f;  // upper arm length (mm)
const float L2 = 100.0f;  // forearm length (mm)
// -------------------

struct Vec2 { float x, y; };

// th1 = shoulder angle, th2 = elbow angle (both degrees)
Vec2 forwardKinematics(float th1, float th2) {
  float t1 = th1 * PI / 180.0f;
  float t2 = th2 * PI / 180.0f;

  Vec2 pos;
  pos.x = L1 * cos(t1) + L2 * cos(t1 + t2);
  pos.y = L1 * sin(t1) + L2 * sin(t1 + t2);
  return pos;
}

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 50;  // 20 Hz

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    float th1 = analogRead(A0) * 180.0f / 1023.0f;  // Convert A0 reading to degrees
    float th2 = analogRead(A1) * 180.0f / 1023.0f;  // Convert A1 reading to degrees

    Vec2 pos = forwardKinematics(th1, th2);

    Serial.print("th1: "); Serial.print(th1, 2);
    Serial.print("  th2: "); Serial.print(th2, 2);
    Serial.print("  x: "); Serial.print(pos.x, 2);
    Serial.print("  y: "); Serial.println(pos.y, 2);

    shoulder.write(constrain((int)th1, 0, 180));
    elbow.write(constrain((int)th2, 0, 180));
  }
}
