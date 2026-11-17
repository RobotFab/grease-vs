// snippet: kalman_1d
// tags: kalman, filter, noise, estimation, sensor, fusion, math, smooth
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 1D Kalman filter for noisy analog sensor — tune Q (process noise) and R (sensor noise)
// prompts: Kalman filter, state estimation, noisy sensor filter, 1D Kalman, optimal filter, sensor fusion single axis

struct Kalman1D {
  float Q;   // process noise covariance — smaller = trust model more
  float R;   // sensor noise covariance — larger = trust sensor less
  float P;   // estimate error covariance
  float x;   // state estimate

  Kalman1D(float q, float r) : Q(q), R(r), P(1.0f), x(0.0f) {}

  float update(float z) {
    P += Q;
    float K = P / (P + R);
    x += K * (z - x);
    P *= (1.0f - K);
    return x;
  }
};

Kalman1D kf(0.01f, 1.0f);  // tweak Q and R for your signal
unsigned long prevMs = 0;

void setup() { Serial.begin(9600); }

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  float raw      = analogRead(A0) * (5.0f / 1023.0f);
  float filtered = kf.update(raw);
  Serial.print("raw: "); Serial.print(raw, 3);
  Serial.print("  filtered: "); Serial.println(filtered, 3);
}
