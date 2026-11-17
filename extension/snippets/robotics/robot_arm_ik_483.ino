// snippet: robot_arm_ik_483
// tags: robot, arm, ik, inverse-kinematics, servo, math, interpolation, smooth
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: 2-DOF IK with smooth interpolation: receive a Cartesian target (x,y) over Serial,
//   solve the inverse kinematics (law of cosines), then slew the shoulder/elbow servos toward the
//   IK solution one step per tick so the arm eases into the new pose instead of snapping to it.
// prompts: IK smooth interpolation, inverse kinematics eased motion, 2-DOF IK slew to target, smooth servo IK, interpolate arm to xy target
// verified: compile-only
// source: kd-generated (id 483)
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE: replace with your actual link lengths ----
const float L1 = 12.0f;  // upper arm length (cm)
const float L2 = 9.0f;   // forearm length (cm)
// ----------------------------------------------------------

Servo shoulder;
Servo elbow;

// current pose vs. the IK-solved target the arm is easing toward
float currentAngle1 = 90.0f, currentAngle2 = 90.0f;
int   targetAngle1  = 90,    targetAngle2  = 90;

unsigned long prevMs = 0;
const unsigned long STEP_MS   = 20;    // 50 Hz interpolation tick
const float         SLEW_RATE = 2.0f;  // degrees per tick (100 deg/s)

// 2-DOF planar IK. Returns true if (x,y) is reachable; writes joint angles in degrees.
bool ik2dof(float x, float y, float& th1, float& th2) {
  float r2 = x * x + y * y;
  float cosT2 = (r2 - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);
  if (cosT2 < -1.0f || cosT2 > 1.0f) return false;  // out of reach

  float t2 = acos(cosT2);                             // elbow-up solution
  float t1 = atan2(y, x) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));

  th1 = t1 * 180.0f / PI;
  th2 = t2 * 180.0f / PI;
  return true;
}

// Ease one joint toward its target by at most SLEW_RATE degrees.
float slew(float current, int target) {
  float diff = target - current;
  if (fabs(diff) <= SLEW_RATE) return target;
  return current + (diff > 0 ? SLEW_RATE : -SLEW_RATE);
}

void setup() {
  shoulder.attach(9);
  elbow.attach(10);
  Serial.begin(9600);
  Serial.println("2-DOF IK with smooth interpolation. Use TARGET <x> <y> (cm) to set the goal.");
}

void loop() {
  // Accept a new Cartesian target at any time; solve IK to get target joint angles.
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.startsWith("TARGET")) {
      String args = command.substring(6);
      args.trim();
      int sp = args.indexOf(' ');
      if (sp > 0) {
        float x = args.substring(0, sp).toFloat();
        float y = args.substring(sp + 1).toFloat();
        float th1, th2;
        if (ik2dof(x, y, th1, th2)) {
          targetAngle1 = constrain((int)th1, 0, 180);
          targetAngle2 = constrain((int)th2, 0, 180);
          Serial.print("Target x="); Serial.print(x);
          Serial.print(" y="); Serial.print(y);
          Serial.print(" -> shoulder="); Serial.print(targetAngle1);
          Serial.print(" elbow="); Serial.println(targetAngle2);
        } else {
          Serial.println("Target out of reach");
        }
      }
    }
  }

  unsigned long now = millis();
  if (now - prevMs >= STEP_MS) {
    prevMs = now;
    currentAngle1 = slew(currentAngle1, targetAngle1);
    currentAngle2 = slew(currentAngle2, targetAngle2);
    shoulder.write((int)currentAngle1);
    elbow.write((int)currentAngle2);
  }
}
