// snippet: pico_206
// tags: pico, rp2040, raspberry-pi
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: RPi Pico: read analog A0, A1, A2 every 100ms
// prompts: multi-channel analog Pico, three ADC RP2040, A0 A1 A2 Raspberry Pi Pico, multiple analog inputs Pico, read three sensors Pi Pico, analog array RP2040
// verified: compile-only
// source: kd-generated (id 206)

int aVals[3] = {0, 0, 0};
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  aVals[0] = analogRead(A0);
  aVals[1] = analogRead(A1);
  aVals[2] = analogRead(A2);

  Serial.print("A0:");
  Serial.print(aVals[0]);
  Serial.print(",A1:");
  Serial.print(aVals[1]);
  Serial.print(",A2:");
  Serial.println(aVals[2]);
}
