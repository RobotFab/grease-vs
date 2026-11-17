// snippet: quadrature_encoder
// tags: encoder, quadrature, interrupt, isr, position, rotation, count, motor
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none (interrupt-based)
// description: Quadrature encoder position counter using hardware interrupts
// prompts: quadrature encoder, motor encoder, wheel encoder position, interrupt encoder count, rotary position counter

const int ENC_A = 2;  // interrupt-capable pin
const int ENC_B = 3;  // interrupt-capable pin

volatile long encoderCount = 0;
unsigned long prevMs = 0;

// On AVR (Uno/Nano/Mega) an ISR is a plain function — no attribute. IRAM_ATTR is
// ESP32-only; using it here is undefined and breaks the build ("expected initializer").
void onEncoderA() {
  if (digitalRead(ENC_B) == HIGH) encoderCount++;
  else                             encoderCount--;
}

void setup() {
  Serial.begin(9600);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onEncoderA, RISING);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  noInterrupts();
  long count = encoderCount;
  interrupts();

  Serial.print("count: "); Serial.println(count);
}
