// snippet: ik_2dof_parallel_delta
// tags: kinematics, delta, parallel, 2dof, ik, inverse-kinematics, robot, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 2-DOF parallel (delta-style) IK — two equal arms driving a single end-effector
// prompts: 2DOF delta robot IK, parallel arm inverse kinematics, delta arm two link, parallel robot servo angles
// Set ARM and ROD lengths to your actual geometry in mm.
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE ----
const float ARM = 80.0f;   // driving arm length (mm)
const float ROD = 120.0f;  // passive rod length  (mm)
const float BASE_OFFSET = 40.0f;  // half-distance between pivot centres
// -------------------

Servo servoL;
Servo servoR;

// Returns shoulder angles in degrees for both arms given target (x=0 centred, y=height)
// Returns false if unreachable
bool deltaIK(float x, float y, float& thL, float& thR) {
  // Left pivot at (-BASE_OFFSET, 0), Right at (+BASE_OFFSET, 0)
  auto solveArm = [&](float pivotX, float& th) -> bool {
    float dx = x - pivotX, dy = y;
    float d  = sqrt(dx * dx + dy * dy);
    if (d > ARM + ROD || d < fabs(ARM - ROD)) return false;
    float a = (ARM * ARM - ROD * ROD + d * d) / (2.0f * ARM * d);
    th = (atan2(dy, dx) - acos(a)) * 180.0f / PI;
    return true;
  };
  return solveArm(-BASE_OFFSET, thL) && solveArm(BASE_OFFSET, thR);
}

void setup() {
  Serial.begin(9600);
  servoL.attach(9); servoR.attach(10);
}

void loop() {
  // scan end-effector left-right at fixed height
  for (float x = -30; x <= 30; x += 5) {
    float thL, thR;
    if (deltaIK(x, 150.0f, thL, thR)) {
      servoL.write(constrain((int)(90 + thL), 0, 180));
      servoR.write(constrain((int)(90 - thR), 0, 180));
      Serial.print("x="); Serial.print(x, 0);
      Serial.print(" thL="); Serial.print(thL, 1);
      Serial.print(" thR="); Serial.println(thR, 1);
    }
    delay(200);
  }
}
