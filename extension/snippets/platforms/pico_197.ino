// snippet: pico_197
// tags: pico, rp2040, raspberry-pi
// boards: rp2040
// library: none
// description: RPi Pico: blink onboard LED on pin 25 at 1Hz using millis
// prompts: blink LED Pico, blink onboard LED RP2040, Pi Pico LED blink 1Hz, blink pin 25 Pico, blink without delay RP2040, LED millis Raspberry Pi Pico
// verified: compile-only
// source: kd-generated (id 197)

unsigned long prevTick = 0;
unsigned long prevBlink = 0;
bool ledState = false;

void setup() {
  pinMode(25, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();

  // Separate timers — sharing one prev variable causes the 500ms block
  // to reset it before the 1000ms block can ever accumulate.
  if (now - prevTick >= 1000) {
    prevTick = now;
    Serial.println("TICK");
  }

  if (now - prevBlink >= 500) {
    prevBlink = now;
    ledState = !ledState;
    digitalWrite(25, ledState);
  }
}
