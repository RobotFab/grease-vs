// snippet: sensor_distance_56
// tags: distance, hcsr04, ultrasonic, trigger, echo
// boards: avr, megaavr, romi32u4
// library: none
// description: GP2Y0A21 IR distance sensor on A0
// prompts: gp2y0a21, distance, sensor, analog, compute, distance
// verified: compile-only
// source: kd-generated (id 56)

const int IR_PIN = A0;
float distCm = 0.0;
unsigned long prevMs = 0;
const unsigned long INTERVAL = 200;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    // Read analog value from IR sensor
    int raw = analogRead(IR_PIN);

    // Compute distance in cm using the given formula
    distCm = 12343.85 / (raw - 3.0);

    // Print distance
    Serial.print("distance: "); Serial.print(distCm); Serial.println(" cm");
  }
}
