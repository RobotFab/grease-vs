// snippet: avr_timer_pwm
// tags: timer, avr, pwm, timer1, hardware, phase-correct, frequency, duty cycle, uno, nano
// boards: avr, romi32u4
// library: none
// description: Hardware PWM on AVR Timer1 (pins 9 and 10 on Uno/Nano) with configurable
//   frequency and duty cycle via direct register manipulation — not analogWrite().
//   Phase-correct mode, /8 prescaler → ~3.9 kHz on 16 MHz.
// prompts: AVR hardware PWM, Timer1 PWM Arduino, PWM frequency AVR, phase correct PWM, direct register PWM, custom PWM frequency Arduino

// Pin 9 = OC1A, Pin 10 = OC1B on Uno / Nano

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;

  // Phase-correct PWM, TOP = ICR1, /8 prescaler
  // f_PWM = F_CPU / (2 * prescaler * TOP) = 16e6 / (2 * 8 * 2000) ≈ 500 Hz
  ICR1   = 2000;
  OCR1A  = 1000; // pin 9 — 50% duty cycle
  OCR1B  = 500;  // pin 10 — 25% duty cycle

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1); // non-inverting on OC1A + OC1B
  TCCR1A |= (1 << WGM11);                   // phase-correct, TOP=ICR1
  TCCR1B |= (1 << WGM13);
  TCCR1B |= (1 << CS11);                    // /8 prescaler
  interrupts();
}

void loop() {
  // Adjust OCR1A 0..ICR1 to change duty cycle at runtime
}
