// snippet: button_toggle
// tags: button, toggle, digital, state, led, press, pullup
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// description: Toggle output state on each button press using millis() edge detection.
//   NOT: delay(20) as a "cheap debounce". It stalls the whole robot for every press and
//   still misses bounce on the RELEASE edge. Timestamp the last accepted edge instead.
//   NOT for the Romi 32U4: pin 2 is its I2C SDA line, and it has three debounced buttons
//   already — use Romi32U4ButtonA and getSingleDebouncedPress().
// prompts: button toggle, toggle on press, toggle LED button, switch state, press to toggle
// not-tags: romi32u4, romi

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

const int BTN = 2;
const int LED = LED_BUILTIN;

const unsigned long DEBOUNCE_MS = 30;

bool lastBtn = HIGH;
bool output  = false;
unsigned long lastEdgeMs = 0;

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
}

void loop() {
  bool btn = digitalRead(BTN);

  // Ignore every edge, press or release, inside the settling window.
  if (btn != lastBtn && millis() - lastEdgeMs >= DEBOUNCE_MS) {
    lastEdgeMs = millis();
    if (btn == LOW) {          // falling edge = press
      output = !output;
      digitalWrite(LED, output);
    }
    lastBtn = btn;
  }
}
