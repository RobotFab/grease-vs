// snippet: ik_poe_nr_3dof
// tags: kinematics, ik, inverse-kinematics, poe, product-of-exponentials, newton-raphson, numerical-ik, jacobian, 3dof, arm
// not-tags: dh, denavit-hartenberg, geometric-ik, analytical-ik, 2dof, planar, romi32u4
// boards: avr, megaavr, nano33iot, rp2040
// library: Servo
// description: 3-DOF numerical inverse kinematics using Product of Exponentials + Newton-Raphson.
//   FK via PoE twist exponentials evaluated at each NR iteration.
//   Numerical Jacobian by finite differences (no symbolic derivatives needed).
//   Position-only IK (XYZ target). For 3-DOF the Jacobian is 3x3 so J-pseudo = J-inverse.
//   Update rule: delta_theta = J_inv * (x_desired - x_current).
//   Converges in 5-15 iterations for reachable targets.
//   Same twist/g0 configuration as fk_poe_3dof — change twists for your arm geometry.
// prompts: numerical IK Newton-Raphson, PoE inverse kinematics, iterative IK solver, Jacobian IK robot arm, Newton-Raphson IK twist, numerical inverse kinematics 3DOF
// exact-include: Servo.h, math.h

#include <Servo.h>
#include <math.h>

// ---- CONFIGURE: geometry (same as FK snippet) ----
#define ARM_L1 100.0f
#define ARM_L2 130.0f
#define ARM_L3 110.0f
#define N_JOINTS 3

const float XI[N_JOINTS][6] = {
  {0, 0, 0,  0, 0, 1},
  {-ARM_L1, 0, 0,  0, 1, 0},
  {-ARM_L1, 0, ARM_L2,  0, 1, 0}
};

float G0[4][4] = {
  {1, 0, 0, ARM_L2 + ARM_L3},
  {0, 1, 0, 0},
  {0, 0, 1, ARM_L1},
  {0, 0, 0, 1}
};

// ---- CONFIGURE: servos ----
Servo servos[N_JOINTS];
const uint8_t SERVO_PINS[] = {9, 10, 11};
const float SERVO_OFFSET[] = {90, 0, 0};  // degrees offset: theta=0 -> servo at this angle

// ---- CONFIGURE: NR solver parameters ----
const int   NR_MAX_ITER = 20;
const float NR_TOL      = 1.0f;     // mm convergence tolerance
const float NR_EPS      = 0.001f;   // radians for numerical Jacobian

// ---- 4x4 matrix helpers ----
void mat4_eye(float M[4][4]) {
  memset(M, 0, 16 * sizeof(float));
  M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1.0f;
}

void mat4_mul(const float A[4][4], const float B[4][4], float C[4][4]) {
  float tmp[4][4];
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) {
      tmp[i][j] = 0;
      for (int k = 0; k < 4; k++)
        tmp[i][j] += A[i][k] * B[k][j];
    }
  memcpy(C, tmp, 16 * sizeof(float));
}

void rodrigues(const float w[3], float th, float R[3][3]) {
  float S[3][3] = {
    { 0,    -w[2],  w[1]},
    { w[2],  0,    -w[0]},
    {-w[1],  w[0],  0   }
  };
  float st = sin(th), ct = cos(th);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      float s2 = S[i][0]*S[0][j] + S[i][1]*S[1][j] + S[i][2]*S[2][j];
      R[i][j] = (i == j ? 1.0f : 0.0f) + st * S[i][j] + (1.0f - ct) * s2;
    }
}

void twist_exp(const float xi[6], float th, float T[4][4]) {
  const float *v = xi, *w = xi + 3;
  float R[3][3];
  rodrigues(w, th, R);
  float wv[3] = {w[1]*v[2]-w[2]*v[1], w[2]*v[0]-w[0]*v[2], w[0]*v[1]-w[1]*v[0]};
  float wdv = w[0]*v[0] + w[1]*v[1] + w[2]*v[2];
  for (int i = 0; i < 3; i++) {
    float Rwv = R[i][0]*wv[0] + R[i][1]*wv[1] + R[i][2]*wv[2];
    T[i][3] = wv[i] - Rwv + w[i]*wdv*th;
    for (int j = 0; j < 3; j++) T[i][j] = R[i][j];
  }
  T[3][0] = T[3][1] = T[3][2] = 0; T[3][3] = 1;
}

// FK: returns 4x4 transform
void computeFK(const float theta[], float T[4][4]) {
  mat4_eye(T);
  for (int j = 0; j < N_JOINTS; j++) {
    float Tj[4][4], tmp[4][4];
    twist_exp(XI[j], theta[j], Tj);
    memcpy(tmp, T, 64); mat4_mul(tmp, Tj, T);
  }
  float tmp[4][4];
  memcpy(tmp, T, 64); mat4_mul(tmp, G0, T);
}

// FK: returns position only
void fkPos(const float theta[], float pos[3]) {
  float T[4][4];
  computeFK(theta, T);
  pos[0] = T[0][3]; pos[1] = T[1][3]; pos[2] = T[2][3];
}

// 3x3 matrix inversion (cofactor method); returns false if singular
bool invert3(const float M[3][3], float Inv[3][3]) {
  float det = M[0][0]*(M[1][1]*M[2][2] - M[1][2]*M[2][1])
            - M[0][1]*(M[1][0]*M[2][2] - M[1][2]*M[2][0])
            + M[0][2]*(M[1][0]*M[2][1] - M[1][1]*M[2][0]);
  if (fabs(det) < 1e-8f) return false;
  float d = 1.0f / det;
  Inv[0][0] =  (M[1][1]*M[2][2] - M[1][2]*M[2][1]) * d;
  Inv[0][1] = -(M[0][1]*M[2][2] - M[0][2]*M[2][1]) * d;
  Inv[0][2] =  (M[0][1]*M[1][2] - M[0][2]*M[1][1]) * d;
  Inv[1][0] = -(M[1][0]*M[2][2] - M[1][2]*M[2][0]) * d;
  Inv[1][1] =  (M[0][0]*M[2][2] - M[0][2]*M[2][0]) * d;
  Inv[1][2] = -(M[0][0]*M[1][2] - M[0][2]*M[1][0]) * d;
  Inv[2][0] =  (M[1][0]*M[2][1] - M[1][1]*M[2][0]) * d;
  Inv[2][1] = -(M[0][0]*M[2][1] - M[0][1]*M[2][0]) * d;
  Inv[2][2] =  (M[0][0]*M[1][1] - M[0][1]*M[1][0]) * d;
  return true;
}

// Newton-Raphson IK: modifies theta in-place; returns true on convergence
bool solveIK(const float target[3], float theta[]) {
  for (int iter = 0; iter < NR_MAX_ITER; iter++) {
    float pos[3];
    fkPos(theta, pos);
    float dx[3] = {target[0]-pos[0], target[1]-pos[1], target[2]-pos[2]};
    float err = sqrt(dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2]);
    if (err < NR_TOL) return true;

    // Numerical Jacobian: J[:,j] = (FK(theta+eps*ej) - FK(theta)) / eps
    float J[3][3];
    for (int j = 0; j < N_JOINTS; j++) {
      float th_p[N_JOINTS];
      memcpy(th_p, theta, N_JOINTS * sizeof(float));
      th_p[j] += NR_EPS;
      float pp[3];
      fkPos(th_p, pp);
      for (int i = 0; i < 3; i++)
        J[i][j] = (pp[i] - pos[i]) / NR_EPS;
    }

    float Jinv[3][3];
    if (!invert3(J, Jinv)) return false;  // near singularity

    for (int j = 0; j < N_JOINTS; j++) {
      float dth = Jinv[j][0]*dx[0] + Jinv[j][1]*dx[1] + Jinv[j][2]*dx[2];
      theta[j] += dth;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < N_JOINTS; i++)
    servos[i].attach(SERVO_PINS[i]);
}

void loop() {
  // Example: reach toward target position (mm)
  float target[3] = {150.0f, 0.0f, 100.0f};
  float theta[N_JOINTS] = {0, PI / 4, PI / 4};  // initial guess

  if (solveIK(target, theta)) {
    for (int i = 0; i < N_JOINTS; i++) {
      int deg = constrain((int)(theta[i] * 180.0f / PI + SERVO_OFFSET[i]), 0, 180);
      servos[i].write(deg);
    }
    Serial.print("IK OK:");
    for (int i = 0; i < N_JOINTS; i++) {
      Serial.print(" "); Serial.print(theta[i] * 180.0f / PI, 1);
    }
    Serial.println(" deg");
  } else {
    Serial.println("IK failed (singular or unreachable)");
  }
  delay(2000);
}
