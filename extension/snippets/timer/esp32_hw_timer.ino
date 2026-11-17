// snippet: esp32_hw_timer
// tags: timer, esp32, hardware timer, interrupt, isr, timerBegin, precise, microsecond
// boards: esp32
// library: none (ESP32 Arduino core)
// description: ESP32 hardware timer interrupt using the ESP32 Arduino core 3.x API.
//   Fires ISR at configurable interval (default 1 ms). Does not rely on delay() or millis().
//   Note: core 3.x API — timerBegin(freq_hz), timerAlarm(timer, ticks, reload, count).
//   The ISR bumps a COUNTER, not a bool flag. A bool silently drops ticks: if loop() takes longer
//   than one period — one Serial.print at 9600 baud will do it — several interrupts collapse into
//   the one flag and the elapsed time you compute from them is short. A counter, incremented in
//   the ISR and drained in loop(), keeps every tick. That matters the moment the tick is driving
//   a control period rather than a blink.
//   Reading a multi-byte volatile that an ISR writes is not atomic: take the value inside
//   noInterrupts()/interrupts() (or portENTER_CRITICAL) so a tick that lands mid-read cannot
//   corrupt it or be lost.
// prompts: ESP32 hardware timer, ESP32 timer interrupt, timerBegin ESP32, ISR ESP32 timer, precise timer ESP32, hw_timer_t ESP32

volatile uint32_t tickCount = 0;   // bumped by the ISR, drained by loop()
hw_timer_t* timer = nullptr;

// An ISR increments one counter and returns. No Serial, no delay(), no I2C, no state machine.
void IRAM_ATTR onTimer() {
  tickCount++;
}

void setup() {
  Serial.begin(115200);
  // timerBegin(frequency_hz) — 1 MHz base clock → ticks in microseconds
  timer = timerBegin(1000000);  // ESP32: create a timer (freq, countUp)
  // Returns nullptr when no timer group is free. Halt rather than run on: attaching an ISR to a
  // null timer crashes, and `return` from setup() would not stop loop() anyway.
  if (timer == nullptr) { Serial.println("timer alloc failed"); while (1); }
  timerAttachInterrupt(timer, &onTimer);  // ESP32: attach an ISR to the timer
  timerAlarm(timer, 1000, true, 0); // fire every 1000 µs = 1 ms, auto-reload
}

void loop() {
  // Take the pending count and clear it in one atomic step, so a tick arriving here is not lost.
  noInterrupts();
  uint32_t pending = tickCount;
  tickCount = 0;
  interrupts();

  if (pending == 0) return;

  static uint32_t elapsedMs = 0;
  elapsedMs += pending;              // every tick counted, even the ones loop() was late for
  if (elapsedMs >= 1000) {
    elapsedMs -= 1000;               // subtract, do not zero — keeps the remainder
    Serial.println("1 s elapsed");
  }
}
