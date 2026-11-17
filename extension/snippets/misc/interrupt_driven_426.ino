// snippet: interrupt_driven_426
// tags: interrupt, attachinterrupt, isr, rising, edge, counter, pin2
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Count rising edges on pin 2 using attachInterrupt()
// prompts: count, rising, edges, attachinterrupt(), every, second
// verified: compile-only
// source: kd-generated (id 426)

volatile unsigned long edgeCount = 0;
unsigned long prevMs = 0;

void onRisingEdge() {
  edgeCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), onRisingEdge, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 1000) {
    prevMs = now;

    noInterrupts();
    unsigned long count = edgeCount;
    edgeCount = 0;
    interrupts();

    float frequencyHz = count / 1.0;
    Serial.print("Frequency: ");
    Serial.print(frequencyHz);
    Serial.println(" Hz");
  }
}
