// snippet: rne_3dof_planar
// tags: dynamics, rne, recursive-newton-euler, inverse-dynamics, torque, 3dof, rrr, robot, planar
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Simplified Recursive Newton-Euler for a 3-DOF planar RRR arm in the XZ plane.
//   Forward pass: propagates angular velocities and linear accelerations base to tip.
//   Backward pass: computes joint torques tip to base.
//   Links modeled as uniform rods (I = m*l^2/12 about center of mass).
//   Gravity is incorporated as a fictitious base acceleration a0 = (0, +g).
//   Useful for torque estimation: compare computed torques with motor current readings
//   to detect overloading or estimate payload weight.
//   All rotations about Y axis; positive theta is counterclockwise in XZ when viewed from -Y.
// prompts: inverse dynamics, joint torque, Newton-Euler dynamics, torque estimation robot arm, RRR dynamics, 3DOF torque computation, recursive Newton-Euler planar
// exact-include: math.h

#include <math.h>

#define N_LINKS 3

// ---- CONFIGURE: link parameters ----
const float LINK_L[N_LINKS] = {0.15f, 0.10f, 0.08f};  // link lengths (m)
const float LINK_M[N_LINKS] = {0.5f,  0.3f,  0.2f};   // link masses (kg)
const float GRAVITY = 9.81f;

// ---- State (set these from encoders/potentiometers) ----
float th[N_LINKS]   = {PI / 4, PI / 6, -PI / 3};  // joint angles (rad)
float dth[N_LINKS]  = {0, 0, 0};                   // joint velocities (rad/s)
float ddth[N_LINKS] = {0, 0, 0};                   // joint accelerations (rad/s^2)

// ---- Intermediate arrays ----
float omega[N_LINKS + 1], alpha[N_LINKS + 1];  // angular vel/accel (scalars)
float jax[N_LINKS + 1], jaz[N_LINKS + 1];      // joint accelerations (2D)
float acx[N_LINKS], acz[N_LINKS];              // CoM accelerations
float rx[N_LINKS], rz[N_LINKS];               // link vectors
float rcx[N_LINKS], rcz[N_LINKS];             // CoM offset vectors
float tau[N_LINKS];                            // output: joint torques (Nm)

// 2D cross product in XZ plane (gives Y-component = torque about Y)
// Positive = counterclockwise when viewed from -Y direction
float cross2d(float ax, float az, float bx, float bz) {
  return ax * bz - az * bx;
}

void forwardPass() {
  omega[0] = 0;  alpha[0] = 0;
  jax[0] = 0;    jaz[0] = GRAVITY;   // fictitious acceleration models gravity

  float phi = 0;
  for (int i = 0; i < N_LINKS; i++) {
    phi += th[i];
    omega[i + 1] = omega[i] + dth[i];
    alpha[i + 1] = alpha[i] + ddth[i];

    rx[i]  = LINK_L[i] * cos(phi);
    rz[i]  = LINK_L[i] * sin(phi);
    rcx[i] = rx[i] * 0.5f;
    rcz[i] = rz[i] * 0.5f;

    float w = omega[i + 1], a = alpha[i + 1];
    // Tangential: (-a*rz, a*rx), Centripetal: (-w^2*rx, -w^2*rz)
    jax[i + 1] = jax[i] - a * rz[i] - w * w * rx[i];
    jaz[i + 1] = jaz[i] + a * rx[i] - w * w * rz[i];

    acx[i] = jax[i] - a * rcz[i] - w * w * rcx[i];
    acz[i] = jaz[i] + a * rcx[i] - w * w * rcz[i];
  }
}

void backwardPass() {
  float fx_next = 0, fz_next = 0;
  float tau_next = 0;

  for (int i = N_LINKS - 1; i >= 0; i--) {
    float m  = LINK_M[i];
    float Ic = m * LINK_L[i] * LINK_L[i] / 12.0f;  // uniform rod about CoM

    float fx = fx_next + m * acx[i];
    float fz = fz_next + m * acz[i];

    tau[i] = tau_next
           + Ic * alpha[i + 1]
           + cross2d(rcx[i], rcz[i], m * acx[i], m * acz[i])
           + cross2d(rx[i], rz[i], fx_next, fz_next);

    fx_next  = fx;
    fz_next  = fz;
    tau_next = tau[i];
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prev = 0;
  if (millis() - prev < 500) return;
  prev = millis();

  // Optional: read joint angles from potentiometers
  // th[0] = (float)map(analogRead(A0), 0, 1023, -90, 90) * PI / 180.0f;
  // th[1] = (float)map(analogRead(A1), 0, 1023,   0, 180) * PI / 180.0f;
  // th[2] = (float)map(analogRead(A2), 0, 1023, -90,  90) * PI / 180.0f;

  forwardPass();
  backwardPass();

  Serial.print("Torques (Nm):");
  for (int i = 0; i < N_LINKS; i++) {
    Serial.print(" J"); Serial.print(i + 1);
    Serial.print("=");  Serial.print(tau[i], 3);
  }
  Serial.println();
}
