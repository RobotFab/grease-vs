// snippet: sensor_hall_578
// tags: hall, magnetic, sensor, analog, linear, a1302, voltage, field
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: none
// description: Read a linear Hall effect sensor (A1302) on A0
// prompts: linear, effect, sensor, (a1302), print, value
// verified: compile-only
// source: kd-generated (id 578)
const int HALL_PIN = A0;
const float SENSITIVITY = 0.0015f;  // V/mT: 0.0015 for A1302

// Board-specific ADC parameters: AVR = 10-bit/5V, others = 12-bit/3.3V
#if defined(ESP32) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_ARCH_RP2040)
  const float VREF = 3.3f;
  const float ADC_MAX = 4095.0f;
#else
  const float VREF = 5.0f;
  const float ADC_MAX = 1023.0f;
#endif

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  int adcValue = analogRead(HALL_PIN);
  float voltage = adcValue * (VREF / ADC_MAX);
  // Zero-field midpoint is half of ADC range
  float midpointV = (ADC_MAX / 2.0f) * (VREF / ADC_MAX);
  float magneticField = (voltage - midpointV) / SENSITIVITY;

  Serial.print("ADC: "); Serial.print(adcValue);
  Serial.print(" mT: "); Serial.println(magneticField, 3);
}
