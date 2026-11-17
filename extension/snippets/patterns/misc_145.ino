// snippet: misc_145
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Non-blocking button debounce on pin 2 (INPUT_PULLUP)
// prompts: non-blocking, button, debounce, (input_pullup), declare, unsigned
// verified: compile-only
// source: kd-generated (id 145)

unsigned long debounceMs = 0;
bool lastState = HIGH;
bool stableState = HIGH;

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  bool currentState = digitalRead(2);

  if (currentState != lastState) {
    debounceMs = now;
  }

  if (now - debounceMs > 50) {
    if (currentState == lastState) {
      stableState = currentState;
    }
  }

  if (stableState == LOW) {
    digitalWrite(13, !digitalRead(13));
    unsigned long tRel = millis();
    while (digitalRead(2) == LOW) {  // wait for release, bounded so it can't hang
      if (millis() - tRel > 1000) break;
    }
  }

  lastState = currentState;
}
