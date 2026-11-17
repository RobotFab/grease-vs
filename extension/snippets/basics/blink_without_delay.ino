// snippet: blink_without_delay
// tags: blink, led, millis, nonblocking, delay, timer, basics, builtin
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Blink LED non-blocking using millis() — the correct pattern to avoid blocking delay()
// prompts: blink without delay, non-blocking LED blink, millis LED, flash LED without blocking, toggle LED interval

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

const int LED = LED_BUILTIN;
const unsigned long INTERVAL = 500;

bool ledState = false;
unsigned long prevMs = 0;

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    ledState = !ledState;
    digitalWrite(LED, ledState);
  }
  // other non-blocking work goes here
}
