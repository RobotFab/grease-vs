// snippet: tinyekf_gps_baro_altitude
// tags: ekf, kalman, tinyekf, gps, barometer, altitude, bmp280, fusion, height
// boards: TBD
// library: TinyEKF, Adafruit BMP280
// description: Fuse GPS altitude (slow, coarse) with barometer (fast, drifts) using TinyEKF.
//   State = [altitude_m, vertical_velocity_m_s].
//   GPS and baro measure altitude; EKF smooths and estimates climb rate.
// prompts: GPS barometer fusion, altitude EKF Kalman, fuse GPS altitude barometer, vertical velocity Kalman, baro GPS height
// exact-include: TinyEKF.h, Wire.h, Adafruit_BMP280.h

#include <TinyEKF.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// State: [alt_m, vel_m_s]   Measurements: [gps_alt_m, baro_alt_m]
class AltEKF : public TinyEKF {
public:
  AltEKF() {
    setQ(0, 0, 0.01f);   // alt process noise
    setQ(1, 1, 0.1f);    // velocity process noise
    setR(0, 0, 5.0f);    // GPS altitude noise (±5m typical)
    setR(1, 1, 0.5f);    // baro altitude noise (±0.5m)
  }

  void model(float* fx, float* F, float* hx, float* H) {
    // Predict: alt += vel * dt
    fx[0] = x[0] + x[1] * dt;
    fx[1] = x[1];  // constant velocity model

    F[0] = 1;  F[1] = dt;
    F[2] = 0;  F[3] = 1;

    // Both sensors observe altitude directly
    hx[0] = x[0];
    hx[1] = x[0];

    H[0] = 1; H[1] = 0;
    H[2] = 1; H[3] = 0;
  }

  float dt = 0.1f;
};

AltEKF ekf;
Adafruit_BMP280 bmp;
unsigned long prevMs = 0;

float seaLevelPressure = 1013.25f;  // hPa — update for your location

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!bmp.begin(0x76)) { Serial.println("BMP280 not found"); while (1); }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_125);
  Serial.println("AltEKF ready");
}

float getGPSAltitude() {
  // Replace with your GPS library call, e.g. gps.altitude.meters()
  return 0.0f;
}

bool gpsUpdated() {
  // Replace with your GPS newNMEA / fix check
  return false;
}

void loop() {
  unsigned long now = millis();
  float dt = (now - prevMs) / 1000.0f;
  if (dt < 0.1f) return;
  prevMs = now;

  float baroAlt = bmp.readAltitude(seaLevelPressure);
  float gpsAlt  = gpsUpdated() ? getGPSAltitude() : ekf.getX(0);  // use predicted if no new fix

  ekf.dt = dt;
  float z[2] = {gpsAlt, baroAlt};
  ekf.step(z);

  float alt = ekf.getX(0);
  float vel = ekf.getX(1);

  Serial.print("alt: "); Serial.print(alt, 2);
  Serial.print(" m  vel: "); Serial.print(vel, 3);
  Serial.println(" m/s");
}
