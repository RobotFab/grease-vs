// snippet: hcsr04_basic
// tags: distance, ultrasonic, hcsr04, hc-sr04, sonar, pulseIn, trigger, echo
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none (raw pulseIn)
// description: Read distance from an HC-SR04 ultrasonic sensor using raw pulseIn.
//   A timed-out pulseIn returns 0, and 0 is NOT a distance — it means no echo came back, which is
//   what an out-of-range reading, an absorbent surface and an angled surface all look like. Report
//   it as "no echo", never as 0 cm: a robot that treats it as 0 cm brakes for open floor.
//   pulseIn BLOCKS for up to its timeout. At 30000 us that is a 30 ms stall of loop() on every
//   reading, so keep the timeout only as long as the range you actually need (30 ms ≈ 5 m) and do
//   not call it faster than 20 Hz — the sensor needs ~60 ms between pings to avoid hearing its own
//   previous echo.
//   NOT a Romi 32U4 sketch: pins 9 and 10 are the Romi's right and left motor PWM outputs. On a
//   Romi, pick free pins and see hcsr04_newping's header for the NewPing timer conflict.
// prompts: HC-SR04 distance, ultrasonic sensor, measure distance, sonar distance, pulseIn distance, obstacle detection, distance in cm
// not-tags: romi32u4, romi

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

// Returns centimetres, or -1 when no echo returned within the timeout.
// -1 is a sentinel the caller must handle; returning 0 would be indistinguishable from a real
// reading of "something touching the sensor".
long readCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Blocks up to 30 ms. Returns 0 on timeout — no echo, not zero distance.
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return duration / 58;   // us / 58 = cm (round trip at ~343 m/s)
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;   // 10 Hz — also gives the sensor time to settle between pings
  prevMs = now;

  long cm = readCm();
  if (cm < 0) {
    Serial.println("distance: no echo");   // out of range / absorbent / angled surface
    return;
  }
  Serial.print("distance: ");
  Serial.print(cm);
  Serial.println(" cm");
}
