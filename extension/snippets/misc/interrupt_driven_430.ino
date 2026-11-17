// snippet: interrupt_driven_430
// tags: interrupt, timer1, icr1, pwm, timer, avr, hardware-timer
// boards: avr, romi32u4
// library: none
// description: Use Timer1 (16-bit) on an Uno to generate a 1 kHz interrupt using ICR1 and Fast PWM mode
// prompts: timer1, (16-bit), generate, interrupt, toggle, library
// verified: compile-only
// source: kd-generated (id 430)

void setup() {
  pinMode(8, OUTPUT);

  // Set Timer1 to Fast PWM mode with ICR1 as top
  TCCR1A = (1 << COM1A1) | (1 << WGM11);  // Clear OC1A on compare match, Fast PWM mode
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // Fast PWM mode, no prescaler (16MHz clock)

  // For 1kHz overflow interrupt with 16MHz clock and no prescaler in Fast PWM mode:
  // overflow freq = F_CPU / (ICR1 + 1), so ICR1 = (F_CPU / frequency) - 1 = (16000000 / 1000) - 1 = 15999
  ICR1 = 15999;  // Correct value for 1kHz overflow with no prescaler (Fast PWM: F_CPU/(ICR1+1))

  // Enable Timer1 overflow interrupt
  TIMSK1 |= (1 << TOIE1);
}

void loop() {
  // Main loop can be empty
}

// Timer1 overflow ISR
ISR(TIMER1_OVF_vect) {
  // Toggle pin 8
  digitalWrite(8, !digitalRead(8));
}
