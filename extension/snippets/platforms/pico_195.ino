// snippet: pico_195
// tags: pico, rp2040, raspberry-pi
// boards: rp2040
// library: none
// description: RPi Pico: read analog on A0 every 100ms using millis
// prompts: read analog Pico, ADC RP2040, analog sensor Raspberry Pi Pico, potentiometer Pico, read voltage Pico, analog input RP2040, analogRead Pico every 100ms
// verified: compile-only
// source: kd-generated (id 195)

int rawVal = 0;
float voltage = 0.0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 100;

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    rawVal = analogRead(A0);
    voltage = rawVal * (3.3 / 1023.0);
    Serial.print("raw: "); Serial.print(rawVal);
    Serial.print("  voltage: "); Serial.println(voltage, 3);
  }
}
