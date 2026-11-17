// snippet: sharp_ir_gp2y
// tags: distance, sharp, ir, infrared, gp2y0a21, gp2y, analog, analog-distance
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read distance from Sharp GP2Y0A21 (10-80 cm) analog IR sensor with voltage-to-cm formula
// prompts: Sharp IR distance sensor, GP2Y0A21, analog distance sensor, infrared range finder, measure distance analog

const int IR_PIN = A1;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;
  prevMs = now;

  // average 5 readings to reduce noise
  long sum = 0;
  for (int i = 0; i < 5; i++) { sum += analogRead(IR_PIN); delay(2); }
  float volts = (sum / 5.0) * (5.0 / 1023.0);

  // GP2Y0A21 empirical formula (valid ~10-80 cm)
  float cm = 27.86 / (volts - 0.42);
  cm = constrain(cm, 5, 100);

  Serial.print("distance: "); Serial.print(cm, 1); Serial.println(" cm");
}
