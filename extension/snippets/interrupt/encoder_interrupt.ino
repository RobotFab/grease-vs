// snippet: encoder_interrupt
// tags: encoder, quadrature, interrupt, isr, motor, count, position, speed
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Quadrature encoder position tracking using two external interrupts (pins 2 & 3)
// prompts: encoder interrupt ISR, motor position interrupt, quadrature interrupt count, speed sensing encoder ISR

const int ENC_A = 2;
const int ENC_B = 3;

volatile long position = 0;

void onA() {
  position += (digitalRead(ENC_B) == LOW) ? 1 : -1;
}

void onB() {
  position += (digitalRead(ENC_A) == HIGH) ? 1 : -1;
}

void setup() {
  Serial.begin(9600);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), onB, CHANGE);
}

void loop() {
  noInterrupts();
  long pos = position;
  interrupts();
  Serial.print("position: "); Serial.println(pos);
  delay(100);
}
