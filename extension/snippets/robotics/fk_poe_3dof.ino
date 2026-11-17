// snippet: fk_poe_3dof
// tags: kinematics, fk, forward-kinematics, poe, product-of-exponentials, twist, rodrigues, 3dof, arm, xyz, screw
// not-tags: dh, denavit-hartenberg
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 3-DOF forward kinematics using Product of Exponentials (PoE) convention.
//   Each joint defined by a 6x1 twist xi = (v, omega): omega is the unit rotation axis,
//   v = -omega x q where q is any point on the axis.
//   FK = e^(xi1*th1) * e^(xi2*th2) * e^(xi3*th3) * g0.
//   Uses Rodrigues formula for the matrix exponential of each twist.
//   Configure joint twists and home config g0 for your arm geometry.
//   Reads joint angles from potentiometers on A0-A2 mapped to configurable degree ranges.
// prompts: product of exponentials, PoE FK, twist forward kinematics, screw theory FK, Rodrigues FK, 3DOF FK PoE, exponential coordinates, twist kinematics
// exact-include: math.h

#include <math.h>

// ---- CONFIGURE: link lengths (mm) ----
#define ARM_L1 100.0f   // base to shoulder (vertical)
#define ARM_L2 130.0f   // upper arm
#define ARM_L3 110.0f   // forearm
#define N_JOINTS 3

// ---- CONFIGURE: potentiometer pins and joint angle ranges (degrees) ----
#if defined(ESP32)
const uint8_t POT_PINS[] = {36, 39, 34};  // ESP32 ADC1 pins
#else
const uint8_t POT_PINS[] = {A0, A1, A2};
#endif
const float TH_MIN[] = {-90, 0, 0};
const float TH_MAX[] = { 90, 180, 180};

// ---- CONFIGURE: 6x1 joint twists at home configuration ----
// xi_i = {v_x, v_y, v_z, omega_x, omega_y, omega_z}
// For a revolute joint with unit axis omega through point q: v = -omega x q
const float XI[N_JOINTS][6] = {
  // Joint 1 (base azimuth): rotate about Z through origin
  //   omega=(0,0,1), q=(0,0,0) => v=(0,0,0)
  {0, 0, 0,  0, 0, 1},
  // Joint 2 (shoulder): rotate about Y through (0, 0, L1)
  //   omega=(0,1,0), q=(0,0,L1) => v = -(0,1,0)x(0,0,L1) = (-L1,0,0)
  {-ARM_L1, 0, 0,  0, 1, 0},
  // Joint 3 (elbow): rotate about Y through (L2, 0, L1) at home
  //   omega=(0,1,0), q=(L2,0,L1) => v = -(0,1,0)x(L2,0,L1) = (-L1,0,L2)
  {-ARM_L1, 0, ARM_L2,  0, 1, 0}
};

// ---- CONFIGURE: home configuration g0 ----
// End-effector pose when all theta = 0: arm extends along +X at height L1
float G0[4][4] = {
  {1, 0, 0, ARM_L2 + ARM_L3},
  {0, 1, 0, 0},
  {0, 0, 1, ARM_L1},
  {0, 0, 0, 1}
};

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

// Rodrigues: R = I + sin(th)*[w]x + (1-cos(th))*[w]x^2
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

// Matrix exponential of twist xi at angle th -> 4x4 transform T
// e^(xi_hat * th) = [R, (I-R)(w x v) + w*wT*v*th; 0 0 0 1]
void twist_exp(const float xi[6], float th, float T[4][4]) {
  const float *v = xi;       // v = xi[0..2]
  const float *w = xi + 3;   // omega = xi[3..5]

  float R[3][3];
  rodrigues(w, th, R);

  // omega x v (cross product)
  float wv[3] = {
    w[1]*v[2] - w[2]*v[1],
    w[2]*v[0] - w[0]*v[2],
    w[0]*v[1] - w[1]*v[0]
  };
  float wdv = w[0]*v[0] + w[1]*v[1] + w[2]*v[2];  // omega . v

  // p = (I - R)(omega x v) + omega*(omega . v)*th
  for (int i = 0; i < 3; i++) {
    float Rwv = R[i][0]*wv[0] + R[i][1]*wv[1] + R[i][2]*wv[2];
    T[i][3] = wv[i] - Rwv + w[i] * wdv * th;
    for (int j = 0; j < 3; j++)
      T[i][j] = R[i][j];
  }
  T[3][0] = T[3][1] = T[3][2] = 0;
  T[3][3] = 1;
}

// FK: g(theta) = e^(xi1*th1) * e^(xi2*th2) * e^(xi3*th3) * g0
void computeFK(const float theta[], float T[4][4]) {
  mat4_eye(T);
  for (int j = 0; j < N_JOINTS; j++) {
    float Tj[4][4];
    twist_exp(XI[j], theta[j], Tj);
    float tmp[4][4];
    memcpy(tmp, T, 16 * sizeof(float));
    mat4_mul(tmp, Tj, T);
  }
  float tmp[4][4];
  memcpy(tmp, T, 16 * sizeof(float));
  mat4_mul(tmp, G0, T);
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prev = 0;
  if (millis() - prev < 200) return;
  prev = millis();

  float theta[N_JOINTS];
  for (int i = 0; i < N_JOINTS; i++) {
    float raw = (float)analogRead(POT_PINS[i]);
    float deg = TH_MIN[i] + (TH_MAX[i] - TH_MIN[i]) * raw / 1023.0f;
    theta[i] = deg * PI / 180.0f;
  }

  float T[4][4];
  computeFK(theta, T);

  Serial.print("X="); Serial.print(T[0][3], 1);
  Serial.print(" Y="); Serial.print(T[1][3], 1);
  Serial.print(" Z="); Serial.println(T[2][3], 1);
}
