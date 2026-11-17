// snippet: interrupt_driven_435
// tags: avr/io, avr/interrupt
// boards: avr, romi32u4
// library: avr/io, avr/interrupt
// description: Use Timer1 Compare Match A interrupt to sample analogRead(A0) every 10 ms precisely
// prompts: timer1, compare, match, interrupt, sample, analogread(a0)
// verified: compile-only
// source: kd-generated (id 435)
// exact-include: avr/io.h, avr/interrupt.h

#include <avr/io.h>
#include <avr/interrupt.h>

#define BUFFER_SIZE 100
float buffer[BUFFER_SIZE];
int index = 0;
volatile bool bufferFull = false;
volatile bool sampleReady = false;  // Flag to indicate sample is ready

void setup() {
  Serial.begin(115200);

  // Initialize Timer1
  TCCR1A = 0;  // Normal mode
  TCCR1B = (1 << CS12) | (1 << CS10);  // Prescaler 1024
  OCR1A = 249;  // 16MHz / 1024 / 250 = 62.5 Hz (10 ms period)
  TIMSK1 |= (1 << OCIE1A);  // Enable Compare Match A interrupt

  // Enable global interrupts
  sei();
}

void loop() {
  static unsigned long lastPrintTime = 0;
  unsigned long now = millis();

  // Read analog value in main loop, not in ISR
  // This avoids blocking the ISR for ~100us while analogRead completes
  if (sampleReady) {
    noInterrupts();
    buffer[index] = analogRead(A0);
    index = (index + 1) % BUFFER_SIZE;
    if (index == 0) {
      bufferFull = true;
    }
    sampleReady = false;
    interrupts();
  }

  if (now - lastPrintTime >= 1000) {
    lastPrintTime = now;

    float sum = 0.0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum += buffer[i];
    }

    float average = sum / BUFFER_SIZE;
    Serial.print("Average: ");
    Serial.println(average, 2);
  }
}

// Timer1 Compare Match A interrupt service routine
// Note: This ISR only sets a flag - analogRead is done in loop() to avoid blocking
ISR(TIMER1_COMPA_vect) {
  sampleReady = true;  // Signal main loop to take a sample
}
