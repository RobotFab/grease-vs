// snippet: multiple_leds
// tags: led, digital, output, array, multiple, sequence, basics
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive multiple LEDs using a pin array — sequential chase pattern
// prompts: multiple LEDs, LED chase, LED array, LED sequence, Knight Rider LEDs, LED strip digital

const int LEDS[]  = {4, 5, 6, 7, 8};
const int N       = sizeof(LEDS) / sizeof(LEDS[0]);
int current = 0;

void setup() {
  for (int i = 0; i < N; i++) pinMode(LEDS[i], OUTPUT);
}

void loop() {
  for (int i = 0; i < N; i++) digitalWrite(LEDS[i], i == current ? HIGH : LOW);
  current = (current + 1) % N;
  delay(150);
}
