// snippet: tinyekf_encoder_imu_pose
// tags: ekf, kalman, tinyekf, encoder, odometry, imu, pose, x, y, heading, robot, fusion
// boards: TBD
// library: TinyEKF
// description: 2D robot pose estimation (x, y, heading) by fusing wheel encoder odometry with
//   IMU yaw. Encoders drift on slippery floors; IMU yaw drifts over time — EKF combines both.
//   State = [x_mm, y_mm, heading_rad].
// prompts: EKF pose estimation, robot 2D position Kalman, encoder IMU fusion EKF, odometry Kalman filter, x y heading EKF
// exact-include: TinyEKF.h

#include <TinyEKF.h>

// ---- CONFIGURE ----
const float WHEEL_DIAM_MM  = 65.0f;
const float TRACK_MM       = 150.0f;   // wheel-to-wheel distance
const int   ENCODER_CPR    = 360;      // counts per revolution (full quad)
// -------------------

// State: [x, y, theta]   Measurement: [enc_theta, imu_theta]
class PoseEKF : public TinyEKF {
public:
  PoseEKF() {
    setQ(0, 0, 1.0f);    // x process noise (mm²)
    setQ(1, 1, 1.0f);    // y process noise
    setQ(2, 2, 0.005f);  // heading process noise (rad²)
    setR(0, 0, 0.01f);   // encoder-derived heading noise
    setR(1, 1, 0.005f);  // IMU yaw noise
  }

  void model(float* fx, float* F, float* hx, float* H) {
    float c = cos(x[2] + dTheta * 0.5f);
    float s = sin(x[2] + dTheta * 0.5f);

    fx[0] = x[0] + dDist * c;
    fx[1] = x[1] + dDist * s;
    fx[2] = x[2] + dTheta;

    // Jacobian (linearized around current state)
    F[0] = 1; F[1] = 0; F[2] = -dDist * s;
    F[3] = 0; F[4] = 1; F[5] =  dDist * c;
    F[6] = 0; F[7] = 0; F[8] = 1;

    // Both measurements observe heading
    hx[0] = x[2];  // encoder-derived heading
    hx[1] = x[2];  // IMU yaw

    H[0] = 0; H[1] = 0; H[2] = 1;
    H[3] = 0; H[4] = 0; H[5] = 1;
  }

  float dDist  = 0;   // incremental distance (mm) since last step
  float dTheta = 0;   // incremental rotation (rad) since last step
};

PoseEKF ekf;

// Encoder counts — incremented by ISR
volatile long encL = 0, encR = 0;
long prevEncL = 0, prevEncR = 0;

unsigned long prevMs = 0;
float imuYaw = 0.0f;  // integrated gyro yaw — replace with your IMU library

void readGyroYawRate(float& gz_rads) {
  // Replace with your IMU library call, e.g. IMU.readGyroscope(gx,gy,gz)
  gz_rads = 0.0f;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  // attach encoder interrupts here (not shown — see encoder_interrupt snippet)
  Serial.println("PoseEKF ready");
}

void loop() {
  unsigned long now = millis();
  float dt = (now - prevMs) / 1000.0f;
  if (dt < 0.02f) return;
  prevMs = now;

  // Encoder odometry
  noInterrupts();
  long dL = encL - prevEncL;
  long dR = encR - prevEncR;
  prevEncL = encL; prevEncR = encR;
  interrupts();

  const float mmPerCount = WHEEL_DIAM_MM * PI / ENCODER_CPR;
  float distL = dL * mmPerCount;
  float distR = dR * mmPerCount;

  float dDist  = (distL + distR) * 0.5f;
  float encDTheta = (distR - distL) / TRACK_MM;   // rad from encoders

  // IMU yaw integration
  float gz;
  readGyroYawRate(gz);
  imuYaw += gz * dt;
  float encTheta = ekf.getX(2) + encDTheta;       // predicted heading from encoders

  // Feed EKF
  ekf.dDist  = dDist;
  ekf.dTheta = encDTheta;

  float z[2] = {encTheta, imuYaw};
  ekf.step(z);

  float x   = ekf.getX(0);
  float y   = ekf.getX(1);
  float hdg = ekf.getX(2) * 180.0f / PI;

  Serial.print("x: "); Serial.print(x, 1);
  Serial.print(" mm  y: "); Serial.print(y, 1);
  Serial.print(" mm  hdg: "); Serial.print(hdg, 1);
  Serial.println("°");
}
