// snippet: millis_pattern_library
// tags: timer, millis, non-blocking, pattern, blink, interval, multiple tasks, reference
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Canonical millis() non-blocking timer pattern — two independent intervals
//   (LED blink at 500 ms, sensor read at 2000 ms) without delay(). Reference for any
//   multi-rate task scheduling without an RTOS.
// prompts: millis non-blocking, blink without delay, multiple intervals millis, Arduino timer pattern, non-blocking delay Arduino, millis pattern reference

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

unsigned long prevLedMs    = 0;
unsigned long prevSensorMs = 0;
bool ledState = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // Task A — blink LED every 500 ms
  if (now - prevLedMs >= 500) {
    prevLedMs = now;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
  }

  // Task B — read sensor every 2000 ms
  if (now - prevSensorMs >= 2000) {
    prevSensorMs = now;
    int val = analogRead(A0);
    Serial.print("sensor: "); Serial.println(val);
  }

  // Additional tasks follow the same pattern:
  // if (now - prevXMs >= INTERVAL_X) { prevXMs = now; doX(); }
}
