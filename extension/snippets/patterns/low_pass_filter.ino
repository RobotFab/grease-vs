// snippet: low_pass_filter
// tags: filter, lowpass, low-pass, ema, exponential, smooth, noise, iir
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Single-pole IIR (exponential moving average) low-pass filter — one line, very fast.
//   ema = ALPHA * raw + (1 - ALPHA) * ema
//   Seed the filter with the FIRST sample instead of 0.0, or the output ramps up from zero over
//   the first few dozen samples and a startup reading looks like a real transient.
//   ALPHA is only meaningful together with the SAMPLE INTERVAL: the same ALPHA at 10 ms and at
//   100 ms are two different filters. Keep the interval fixed (the millis() gate below) and
//   state it next to ALPHA. Cutoff ≈ ALPHA / (2 * PI * dt) Hz for small ALPHA.
//   Scale the ADC count with the resolution and reference of the TARGET board, not a hard-coded
//   5.0/1023: an ESP32 is 12-bit over ~3.3 V (4095 counts) and a Nano 33 IoT / SAMD is 3.3 V.
//   Using the AVR constants on those boards reports voltages that are ~50% too high.
// prompts: low pass filter, exponential moving average, EMA filter, smooth sensor noise, IIR filter, noise reduction, signal smoothing, filter analog input, exponential smoothing

// ADC scaling for the target board (see rule_adc_resolution).
#if defined(ESP32)
  const float ADC_VREF   = 3.3f;
  const float ADC_COUNTS = 4095.0f;   // analogReadResolution(12) is the ESP32 default
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED)
  const float ADC_VREF   = 3.3f;
  const float ADC_COUNTS = 1023.0f;   // 10-bit by default; raise with analogReadResolution(12)
#else
  const float ADC_VREF   = 5.0f;      // classic AVR
  const float ADC_COUNTS = 1023.0f;
#endif

// alpha = 0.0-1.0: smaller = smoother but more lag; 0.1-0.3 is typical AT THIS interval
const float ALPHA = 0.1f;
const unsigned long SAMPLE_MS = 10;   // fixed sample interval — ALPHA is tuned against it

float ema = 0.0f;
bool  firstSample = true;
unsigned long prevMs = 0;

void setup() { Serial.begin(9600); }

void loop() {
  unsigned long now = millis();
  if (now - prevMs < SAMPLE_MS) return;
  prevMs = now;

  float raw = analogRead(A0) * (ADC_VREF / ADC_COUNTS);

  if (firstSample) { ema = raw; firstSample = false; }
  else             { ema = ALPHA * raw + (1.0f - ALPHA) * ema; }

  Serial.print("raw: "); Serial.print(raw, 3);
  Serial.print("  ema: "); Serial.println(ema, 3);
}
