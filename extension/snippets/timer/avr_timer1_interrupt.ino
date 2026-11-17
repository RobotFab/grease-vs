// snippet: avr_timer1_interrupt
// tags: timer, avr, timer1, interrupt, isr, hardware, CTC, prescaler, uno, nano, mega
// boards: avr
// library: none
// description: Hardware Timer1 CTC interrupt on AVR (Uno/Nano/Mega) — fires ISR at a precise
//   interval set by OCR1A. Does not use delay() or millis(); good for sampling ticks, audio ticks.
//   NOT for the Romi 32U4 / Pololu A-Star: Romi32U4Motors drives the wheel PWM from Timer1,
//   and writing TCCR1A/TCCR1B/OCR1A/TIMSK1 here takes the timer away from it — the wheels stop
//   responding to setSpeeds(). This is the same conflict that makes Servo.h unusable on a Romi.
//   On a Romi use Timer3 (pin 5 is its only free PWM pin) or a millis() scheduler instead.
//   OCR1A below assumes a 16 MHz clock; recompute for 8 MHz boards.
// prompts: AVR Timer1 interrupt, hardware timer Arduino Uno, CTC mode Timer1, ISR timer AVR, precise timer Arduino, Timer1 ISR
// not-tags: romi32u4, romi, esp32, nano33iot, pico

volatile bool tick = false;

ISR(TIMER1_COMPA_vect) {
  tick = true;
}

void setup() {
  Serial.begin(9600);

  // CTC mode, 1 Hz on 16 MHz clock with /1024 prescaler
  // OCR1A = (F_CPU / prescaler / freq) - 1 = (16000000 / 1024 / 1) - 1 = 15624
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A  = 15624;               // compare match value for 1 Hz
  TCCR1B |= (1 << WGM12);       // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // /1024 prescaler
  TIMSK1 |= (1 << OCIE1A);      // enable compare interrupt
  interrupts();
}

void loop() {
  if (tick) {
    tick = false;
    Serial.println("Timer1 tick");
  }
}
