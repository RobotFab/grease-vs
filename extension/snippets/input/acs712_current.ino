// snippet: acs712_current
// tags: current, acs712, hall-effect, analog, sensor, 5a, 20a, 30a, ac, dc
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: none
// description: Measure AC/DC current with ACS712 hall-effect sensor — calibrate zero-point offset
// prompts: ACS712 current sensor, measure AC DC current, current sensing hall effect, amp meter Arduino, current clamp sensor

const int ACS_PIN     = A0;
const float VREF      = 5.0f;
const float SENSITIVITY = 0.185f;  // V/A: 0.185 for 5A, 0.100 for 20A, 0.066 for 30A
float zeroPoint       = 2.5f;      // calibrate by reading no-load voltage

unsigned long prevMs = 0;

void calibrateZero() {
  long sum = 0;
  for (int i = 0; i < 100; i++) { sum += analogRead(ACS_PIN); delay(1); }
  zeroPoint = (sum / 100.0f) * (VREF / 1023.0f);
  Serial.print("Zero calibrated: "); Serial.println(zeroPoint, 3);
}

void setup() {
  Serial.begin(9600);
  calibrateZero();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  float v = analogRead(ACS_PIN) * (VREF / 1023.0f);
  float amps = (v - zeroPoint) / SENSITIVITY;
  Serial.print("Current: "); Serial.print(amps, 3); Serial.println(" A");
}
