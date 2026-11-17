// snippet: median_filter
// tags: filter, median, noise, spike, outlier, sensor, smooth, sort
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 5-point median filter to eliminate spike noise from analog sensors
// prompts: median filter, remove spikes, outlier rejection, eliminate noise spikes, robust sensor filtering, spike rejection

const int WINDOW = 5;
int buf[WINDOW];
int idx = 0;

int medianFilter(int newVal) {
  buf[idx] = newVal;
  idx = (idx + 1) % WINDOW;

  int tmp[WINDOW];
  memcpy(tmp, buf, sizeof(buf));
  // insertion sort
  for (int i = 1; i < WINDOW; i++) {
    int key = tmp[i], j = i - 1;
    while (j >= 0 && tmp[j] > key) { tmp[j + 1] = tmp[j]; j--; }
    tmp[j + 1] = key;
  }
  return tmp[WINDOW / 2];
}

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < WINDOW; i++) buf[i] = analogRead(A0);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  prevMs = now;

  int raw      = analogRead(A0);
  int filtered = medianFilter(raw);
  Serial.print("raw: "); Serial.print(raw);
  Serial.print("  median: "); Serial.println(filtered);
}
