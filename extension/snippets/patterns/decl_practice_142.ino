// snippet: decl_practice_142
// tags: globals, declare, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Declare float distCm=0.0, unsigned long echoStart=0, bool waitingEcho=false at global scope
// prompts: declare, float, distcm=0, unsigned, echostart=0, waitingecho=false
// verified: compile-only
// source: kd-generated (id 142)

float distCm = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);  // TRIG
  pinMode(10, INPUT);  // ECHO
}

void loop() {
  // HC-SR04: trigger a 10µs pulse, then measure the duration of the HIGH echo pulse
  digitalWrite(9, LOW);
  delayMicroseconds(2);
  digitalWrite(9, HIGH);
  delayMicroseconds(10);
  digitalWrite(9, LOW);
  long duration = pulseIn(10, HIGH, 30000);  // wait up to 30ms for the echo HIGH pulse
  if (duration > 0) {
    distCm = duration / 58.0;  // speed of sound ≈ 58µs per cm (round-trip)
    Serial.print("distance: ");
    Serial.println(distCm);
  } else {
    distCm = 0.0;
    Serial.println("timeout");
  }
  delay(100);  // debounce between readings
}
