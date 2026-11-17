// snippet: interrupt_driven_437
// tags: interrupt, attachinterrupt, isr, timeout, millis, rising
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Detect a timeout: start a millis() timer when a RISING edge is detected on pin 2
// prompts: detect, timeout, start, millis(), timer, rising
// verified: compile-only
// source: kd-generated (id 437)

const int INPUT_PIN = 2;
const unsigned long TIMEOUT_MS = 2000;

unsigned long lastEdgeMs = 0;

void setup() {
  pinMode(INPUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  static bool lastReading = HIGH;
  int reading = digitalRead(INPUT_PIN);

  if (reading != lastReading) {
    if (reading == LOW) {  // Rising edge detected
      lastEdgeMs = millis();
    } else {  // Falling edge detected
      unsigned long now = millis();
      if (now - lastEdgeMs >= TIMEOUT_MS) {
        Serial.println("TIMEOUT");
      }
    }
  }

  lastReading = reading;
}
