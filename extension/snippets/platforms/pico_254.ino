// snippet: pico_254
// tags: pico, rp2040, raspberry-pi
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: RPi Pico: stream A0, A1, A2 as labeled CSV to serial plotter at 50Hz using millis
// prompts: serial plotter Pico, stream sensor data RP2040, labeled CSV serial Raspberry Pi Pico, 50Hz data stream Pi Pico, serial plotter 50Hz RP2040, analog streaming Pico
// verified: compile-only
// source: kd-generated (id 254)

int a0, a1, a2;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 20;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;
    a0 = analogRead(A0);
    a1 = analogRead(A1);
    a2 = analogRead(A2);
    Serial.print("A0:"); Serial.print(a0);
    Serial.print(" A1:"); Serial.print(a1);
    Serial.print(" A2:"); Serial.println(a2);
  }
}
