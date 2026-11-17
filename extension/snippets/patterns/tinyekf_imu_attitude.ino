// snippet: tinyekf_imu_attitude
// tags: ekf, kalman, tinyekf, imu, attitude, roll, pitch, accelerometer, gyroscope, fusion
// boards: TBD
// library: TinyEKF
// description: Estimate roll and pitch from accel+gyro using TinyEKF — fuses noisy accel angles
//   with gyro integration. State = [roll, pitch]. Measurement = accel-derived angles.
//   Tune Q (process noise) and R (sensor noise) for your IMU.
// prompts: IMU attitude EKF, roll pitch Kalman filter, accel gyro fusion EKF, tilt estimation Kalman, TinyEKF attitude estimation
// exact-include: TinyEKF.h

#include <TinyEKF.h>

// State: [roll_rad, pitch_rad]   Measurement: [accel_roll_rad, accel_pitch_rad]
class AttitudeEKF : public TinyEKF {
public:
  AttitudeEKF() {
    // Process noise — how much we trust the gyro integration
    setQ(0, 0, 0.001f);
    setQ(1, 1, 0.001f);
    // Measurement noise — how much we trust accel-derived angles (noisier)
    setR(0, 0, 0.03f);
    setR(1, 1, 0.03f);
  }

  // Predict: integrate gyro rates over dt
  void model(float* fx, float* F, float* hx, float* H) {
    fx[0] = x[0] + gyroX * dt;  // roll
    fx[1] = x[1] + gyroY * dt;  // pitch

    // Jacobian F = I (linear prediction step)
    F[0] = 1; F[1] = 0;
    F[2] = 0; F[3] = 1;

    // Measurement model: h(x) = x (we observe state directly via accel)
    hx[0] = x[0];
    hx[1] = x[1];

    H[0] = 1; H[1] = 0;
    H[2] = 0; H[3] = 1;
  }

  float gyroX = 0, gyroY = 0;
  float dt = 0.02f;  // seconds — match your loop interval
};

AttitudeEKF ekf;
unsigned long prevMs = 0;

// Replace with your actual IMU reads (e.g. LSM6DS3, MPU6050)
void readIMU(float& ax, float& ay, float& az,
             float& gx, float& gy, float& gz) {
  // stub — substitute your IMU library calls here
  ax = 0.0f; ay = 0.0f; az = 1.0f;
  gx = 0.0f; gy = 0.0f; gz = 0.0f;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
}

void loop() {
  unsigned long now = millis();
  float dt = (now - prevMs) / 1000.0f;
  if (dt < 0.02f) return;
  prevMs = now;

  float ax, ay, az, gx, gy, gz;
  readIMU(ax, ay, az, gx, gy, gz);

  // Accel-derived angle (noisy but drift-free)
  float accelRoll  = atan2(ay, az);
  float accelPitch = atan2(-ax, sqrt(ay * ay + az * az));

  // Update EKF
  ekf.dt    = dt;
  ekf.gyroX = gx;
  ekf.gyroY = gy;

  float z[2] = {accelRoll, accelPitch};
  ekf.step(z);

  float roll  = ekf.getX(0) * 180.0f / PI;
  float pitch = ekf.getX(1) * 180.0f / PI;

  Serial.print("roll: ");  Serial.print(roll,  2);
  Serial.print("  pitch: "); Serial.println(pitch, 2);
}
