// snippet: nonblocking_timer
// tags: timer, millis, nonblocking, non-blocking, interval, delay, prevMs, timing
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Multi-rate non-blocking timing template using millis() — never use delay()
// prompts: non-blocking delay, millis timer, blink without delay, periodic task, run every N milliseconds, interval timer, millis timing, avoid delay, scheduled task

// IMPORTANT: always declare prevMs at global/file scope (NOT inside loop)
unsigned long prevMs100  = 0;
unsigned long prevMs500  = 0;
unsigned long prevMs1000 = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  // 100 Hz task
  if (now - prevMs100 >= 10) {
    prevMs100 = now;
    // fast sensor read here
  }

  // 2 Hz task
  if (now - prevMs500 >= 500) {
    prevMs500 = now;
    Serial.print("t="); Serial.println(now);
  }

  // 1 Hz task
  if (now - prevMs1000 >= 1000) {
    prevMs1000 = now;
    // slow task here
  }
}
