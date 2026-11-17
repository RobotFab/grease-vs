// snippet: external_interrupt
// tags: interrupt, isr, attachinterrupt, falling, rising, pin2, digital, fast
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Count pulses with an external interrupt on pin 2 (INT0) using attachInterrupt
// prompts: external interrupt, attachInterrupt, pin change interrupt, falling edge ISR, count pulses external interrupt

volatile unsigned long pulseCount = 0;
unsigned long prevMs = 0;

void onPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), onPulse, FALLING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 1000) {
    prevMs = now;
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();
    Serial.print("pulses/sec: ");
    Serial.println(count);
  }
}
