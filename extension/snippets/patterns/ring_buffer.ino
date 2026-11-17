// snippet: ring_buffer
// tags: ring-buffer, circular, queue, fifo, buffer, data, pattern
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Fixed-size ring buffer (circular FIFO queue) for streaming sensor data.
//   NO #include needed — this is a plain struct, zero dependencies, works on all boards.
//   DO NOT use: #include <RingBuffer.h> — that header does not exist in the RingBuffer library.
//   DO NOT use: #include <RingBuf.h> — the installed library has a different API than this pattern.
//   Just copy the struct below — no library, no header.
// prompts: ring buffer, circular buffer, FIFO queue, streaming data buffer, fixed size buffer, circular FIFO, data recorder ring buffer, log ring buffer, RingBuffer HC-SR04, RingBuffer sensor samples

#define BUF_SIZE 16

struct RingBuf {
  float data[BUF_SIZE];
  int head = 0, tail = 0, count = 0;

  bool push(float v) {
    if (count == BUF_SIZE) return false;  // full
    data[head] = v;
    head = (head + 1) % BUF_SIZE;
    count++;
    return true;
  }

  bool pop(float& v) {
    if (count == 0) return false;  // empty
    v = data[tail];
    tail = (tail + 1) % BUF_SIZE;
    count--;
    return true;
  }

  bool full()  const { return count == BUF_SIZE; }
  bool empty() const { return count == 0; }
};

RingBuf rb;
unsigned long prevMs = 0;

void setup() { Serial.begin(9600); }

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  rb.push(analogRead(A0) * (5.0f / 1023.0f));

  float v;
  while (rb.pop(v)) {
    Serial.println(v, 3);
  }
}
