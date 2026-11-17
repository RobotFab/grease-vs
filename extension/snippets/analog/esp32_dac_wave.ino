// snippet: esp32_dac_wave
// tags: esp32, dac, analog-output, dacwrite, waveform, sawtooth, voltage, gpio25
// boards: esp32
// library: none
// description: True analog output on ESP32 DAC pin 25 (8-bit, GPIO25/GPIO26 only) — sawtooth wave
// prompts: ESP32 DAC output, dacWrite ESP32, analog voltage ESP32, sawtooth wave DAC, GPIO25 analog out, ESP32 waveform generator
// exact-include: none

const int DAC_PIN = 25;            // classic ESP32: DAC1 = GPIO25, DAC2 = GPIO26
const unsigned long STEP_MS = 10;  // one 0-255 ramp every ~2.5 s

unsigned long prevMs = 0;
int level = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < STEP_MS) return;
  prevMs = now;

  dacWrite(DAC_PIN, level);        // 0-255 -> 0-3.3 V
  level = (level + 1) & 0xFF;      // wrap for a sawtooth
}
