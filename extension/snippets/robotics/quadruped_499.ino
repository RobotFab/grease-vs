// snippet: quadruped_499
// tags: quadruped, robot, gait, servo, legs, walking, ik, inverse-kinematics, 3dof, coxa, femur, tibia, foot, target, position
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Per-leg 3-DOF IK for a quadruped: given a foot target (x,y,z) relative to the hip,
//   compute coxa, femur, and tibia servo angles.
//   Coxa angle: atan2(y, x) — rotates leg left/right in horizontal plane.
//   Femur + tibia: 2-link planar IK using law of cosines in the sagittal plane.
//   acos() domain: argument must be clamped to [-1, 1] or acos() returns NaN when foot is out of reach.
//   This is board-agnostic math — integrate with PCA9685 writeMicroseconds() to drive actual servos.
//   NOT: analogWrite() for servo angles — use PCA9685 or Servo.h; map degrees to pulse width.
// prompts: quadruped IK, quadruped inverse kinematics, 3-DOF leg IK, coxa femur tibia angles, quadruped foot target, per-leg IK quadruped, quadruped leg solve, 12-servo quadruped IK, quadruped joint angles, foot position to servo angle, quadruped walk IK, quadruped ik esp32
// exact-include: math.h

#include <math.h>

const float L_FEMUR = 10.0f;  // femur length in cm
const float L_TIBIA = 9.0f;   // tibia length in cm

// Computes coxa angle from foot target position (x, y)
float computeCoxaAngle(float x, float y) {
  return atan2(y, x) * 180.0f / M_PI;
}

// Solves 2-link IK for femur and tibia angles in the sagittal plane
void solve2LinkIK(float x, float z, float &femurAngle, float &tibiaAngle) {
  float r = sqrt(x * x + z * z);

  // Interior knee angle (law of cosines). Clamp so acos() never returns NaN out of reach.
  float cosTibia = (L_FEMUR * L_FEMUR + L_TIBIA * L_TIBIA - r * r) / (2 * L_FEMUR * L_TIBIA);
  cosTibia = constrain(cosTibia, -1.0f, 1.0f);
  tibiaAngle = acos(cosTibia) * 180.0f / M_PI;

  // Femur angle = direction to the foot target + the interior angle between the femur
  // and the femur→foot line. The direction term atan2(z, x) is what makes the leg
  // actually track the target; without it the angle is independent of foot direction.
  float cosFemurOffset = (r * r + L_FEMUR * L_FEMUR - L_TIBIA * L_TIBIA) / (2 * r * L_FEMUR);
  cosFemurOffset = constrain(cosFemurOffset, -1.0f, 1.0f);
  femurAngle = (atan2(z, x) + acos(cosFemurOffset)) * 180.0f / M_PI;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Example foot target position (x, y, z) relative to the hip
  float x = 10.0f;  // x-coordinate in cm
  float y = 0.0f;   // y-coordinate in cm
  float z = 15.0f;  // z-coordinate in cm

  float coxaAngle = computeCoxaAngle(x, y);
  float femurAngle, tibiaAngle;
  solve2LinkIK(x, z, femurAngle, tibiaAngle);

  Serial.print("Coxa Angle: "); Serial.println(coxaAngle, 2);
  Serial.print("Femur Angle: "); Serial.println(femurAngle, 2);
  Serial.print("Tibia Angle: "); Serial.println(tibiaAngle, 2);

  delay(1000);  // Delay for 1 second
}
