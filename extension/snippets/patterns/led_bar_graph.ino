// snippet: led_bar_graph
// tags: led, bar-graph, level, analog, potentiometer, display, meter, indicator
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Show an analog level as a bar graph across a row of LEDs (more LEDs lit = higher reading)
// prompts: LED bar graph, level meter LEDs, show potentiometer on LEDs, analog value LED bar, VU meter LEDs, light more LEDs with knob
// exact-include: none

const int LED_PINS[] = {2, 3, 4, 5};
const int NUM_LEDS = 4;
#ifdef ARDUINO_ARCH_ESP32
const int SENSOR_PIN = 34;    // ADC-capable GPIO on ESP32
const int ADC_MAX = 4095;     // 12-bit ADC
#else
const int SENSOR_PIN = A0;
const int ADC_MAX = 1023;     // 10-bit ADC
#endif
const unsigned long UPDATE_MS = 100;

unsigned long prevMs = 0;

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) pinMode(LED_PINS[i], OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < UPDATE_MS) return;
  prevMs = now;

  int reading = analogRead(SENSOR_PIN);
  int level = map(reading, 0, ADC_MAX, 0, NUM_LEDS + 1);  // 0..NUM_LEDS LEDs lit
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], i < level ? HIGH : LOW);
  }
}
