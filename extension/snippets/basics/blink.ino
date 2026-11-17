// snippet: blink
// tags: blink, led, digital, output, basics, builtin, onboard
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Blink the built-in LED — the canonical Arduino hello-world
// prompts: blink LED, flash LED, turn LED on off, LED blink, digitalWrite LED, built-in LED, onboard LED, toggle LED

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
