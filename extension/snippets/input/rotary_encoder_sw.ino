// snippet: rotary_encoder_sw
// tags: encoder, rotary, knob, menu, switch, interrupt, direction, debounce
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read rotary encoder direction and click using interrupts — menu navigation pattern
// prompts: rotary encoder knob, menu navigation encoder, encoder direction click, volume knob, rotary input scroll wheel

const int ENC_A  = 2;
const int ENC_B  = 3;
const int ENC_SW = 4;

volatile int  position = 0;
volatile int  lastA    = HIGH;
bool          lastSW   = HIGH;

// AVR ISRs are plain functions — no IRAM_ATTR (that attribute is ESP32-only and
// undefined on AVR, which breaks the build with "expected initializer").
void onTurn() {
  int a = digitalRead(ENC_A);
  int b = digitalRead(ENC_B);
  if (a != lastA) {
    position += (a != b) ? 1 : -1;
    lastA = a;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onTurn, CHANGE);
}

void loop() {
  static int lastPos = 0;
  noInterrupts(); int pos = position; interrupts();
  if (pos != lastPos) {
    Serial.print("position: "); Serial.println(pos);
    lastPos = pos;
  }

  bool sw = digitalRead(ENC_SW);
  if (sw == LOW && lastSW == HIGH) { Serial.println("click"); delay(50); }
  lastSW = sw;
}
