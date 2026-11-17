// snippet: decl_practice_244
// tags: globals, declare, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Declare volatile bool isrFired=false and unsigned long isrTime=0 globally
// prompts: declare, volatile, isrfired=false, unsigned, isrtime=0, globally
// verified: compile-only
// source: kd-generated (id 244)

volatile bool isrFired = false;
unsigned long isrTime = 0;

void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, RISING);
}

void loop() {
  if (isrFired) {
    processEvent(isrTime);
    isrFired = false;
  }
}

void handleInterrupt() {
  isrFired = true;
  isrTime = micros();
}

void processEvent(unsigned long eventTime) {
  Serial.print("Interrupt fired at: ");
  Serial.println(eventTime);
}
