// snippet: interrupt_driven_431
// tags: avr/io, avr/interrupt
// boards: avr
// library: avr/io, avr/interrupt
// description: Generate a software PWM on pin 8 using Timer2 CTC interrupts at 20 kHz (50 µs period)
// prompts: generate, software, timer2, interrupts, period), control
// verified: compile-only
// source: kd-generated (id 431)
// exact-include: avr/io.h, avr/interrupt.h

#include <avr/io.h>
#include <avr/interrupt.h>

const int PWM_PIN = 8;
volatile uint8_t dutyCycle = 128;  // Initial duty cycle of 50%

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  Serial.begin(9600);

  // Timer2 setup for CTC mode
  TCCR2A = (1 << WGM21);  // CTC mode
  TCCR2B = (1 << CS22) | (1 << CS21);  // Prescaler 256
  // For 20kHz with 16MHz clock and prescaler 256:
  // OCR2A = (F_CPU / (2 * frequency * prescaler)) - 1 = (16000000 / (2 * 20000 * 256)) - 1 = 1.95 - 1 ≈ 1
  // Using OCR2A=1 gives ~31.25kHz, OCR2A=2 gives ~20.83kHz
  // For 20kHz target, use OCR2A=2 (actual: 16MHz/(256*3) = 20.83kHz)
  OCR2A = 2;  // Correct value for ~20kHz with prescaler 256
  TIMSK2 |= (1 << OCIE2A);  // Enable CTC interrupt
  sei();  // Enable global interrupts
}

ISR(TIMER2_COMPA_vect) {
  static uint8_t count = 0;
  count++;
  if (count >= 3) count = 0;  // Reset count (OCR2A+1 = 3)

  if (count < dutyCycle) {
    digitalWrite(PWM_PIN, HIGH);
  } else {
    digitalWrite(PWM_PIN, LOW);
  }
}

void loop() {
  if (Serial.available() > 0) {
    dutyCycle = Serial.parseInt();
    if (dutyCycle > 255) dutyCycle = 255;
    if (dutyCycle < 0) dutyCycle = 0;
  }
}
