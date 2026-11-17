// snippet: robot_arm_ik_476
// tags: robot, arm, ik, inverse-kinematics, servo, math
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: IK with gripper: solve 2-DOF IK for a target position (x,y), drive shoulder and elbow servos on p…
// prompts: gripper, solve, 2-dof, target, position, drive
// verified: compile-only
// source: kd-generated (id 476)
// exact-include: Servo.h

#include <Servo.h>

Servo shoulder;
Servo elbow;
Servo gripper;

const float L1 = 12.0f;  // Length of first link in cm
const float L2 = 9.0f;   // Length of second link in cm

int currentShoulder = 90, targetShoulder = 90;
int currentElbow = 90, targetElbow = 90;
unsigned long prevMs = 0;
const unsigned long STEP_MS = 10;  // 10ms step for 90deg/sec max

// 2-DOF planar IK. Returns true if (x,y) is reachable; writes joint angles in degrees.
bool ik2dof(float x, float y, float& th1, float& th2) {
  float r2 = x * x + y * y;
  float cosT2 = (r2 - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);
  if (cosT2 < -1.0f || cosT2 > 1.0f) return false;  // out of reach
  float t2 = acos(cosT2);
  float t1 = atan2(y, x) - atan2(L2 * sin(t2), L1 + L2 * cos(t2));
  th1 = t1 * 180.0f / PI;
  th2 = t2 * 180.0f / PI;
  return true;
}

void setup() {
  Serial.begin(9600);
  shoulder.attach(9);
  elbow.attach(10);
  gripper.attach(11);
  shoulder.write(currentShoulder);
  elbow.write(currentElbow);
  gripper.write(0);
  Serial.println("IK with gripper. Use TARGET <x> <y> (cm) to set the foot position. Use O/C to open/close gripper.");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= STEP_MS) {
    prevMs = now;

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
            targetShoulder = constrain((int)th1, 0, 180);
            targetElbow = constrain((int)th2, 0, 180);
            Serial.print("Target x="); Serial.print(x);
            Serial.print(" y="); Serial.print(y);
            Serial.print(" -> shoulder="); Serial.print(targetShoulder);
            Serial.print(" elbow="); Serial.println(targetElbow);
          } else {
            Serial.println("Target out of reach");
          }
        }
      } else if (command == "O") {
        gripper.write(0);
        Serial.println("Gripper opened.");
      } else if (command == "C") {
        gripper.write(90);
        Serial.println("Gripper closed.");
      }
    }

    // Slew both joints one degree per step toward their IK targets.
    if (currentShoulder != targetShoulder) {
      currentShoulder += (targetShoulder > currentShoulder) ? 1 : -1;
      shoulder.write(currentShoulder);
    }
    if (currentElbow != targetElbow) {
      currentElbow += (targetElbow > currentElbow) ? 1 : -1;
      elbow.write(currentElbow);
    }
  }
}
