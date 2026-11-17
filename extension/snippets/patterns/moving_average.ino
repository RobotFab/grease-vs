// snippet: moving_average
// tags: filter, moving average, average, smooth, noise, adc, sensor, window
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Circular buffer moving average filter for sensor smoothing
// prompts: moving average, rolling average, sliding window average, smooth ADC readings, average sensor samples, running average, filter sensor noise

#define WINDOW 16
float buf[WINDOW];
int   idx = 0;
float sum = 0.0;
bool  filled = false;

float movingAvg(float newVal) {
  sum -= buf[idx];
  buf[idx] = newVal;
  sum += newVal;
  idx = (idx + 1) % WINDOW;
  if (idx == 0) filled = true;
  int count = filled ? WINDOW : idx;
  return sum / count;
}

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  memset(buf, 0, sizeof(buf));
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  prevMs = now;

  float raw  = analogRead(A0);
  float smooth = movingAvg(raw);
  Serial.print("raw: ");    Serial.print(raw);
  Serial.print(" smooth: "); Serial.println(smooth);
}
