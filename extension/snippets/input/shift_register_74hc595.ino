// snippet: shift_register_74hc595
// tags: shift-register, 74hc595, shiftout, spi, output-expand, leds, bits
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive 8 outputs from 3 pins using 74HC595 shift register with shiftOut()
// prompts: 74HC595 shift register, expand digital outputs, shiftOut LED control, chain shift registers, 8 outputs from 3 pins

const int DS   = 11;  // data (SER)
const int SHCP = 12;  // shift clock (SRCLK)
const int STCP = 10;  // latch (RCLK)

void writeShift(byte data) {
  digitalWrite(STCP, LOW);
  shiftOut(DS, SHCP, MSBFIRST, data);
  digitalWrite(STCP, HIGH);
}

void setup() {
  pinMode(DS, OUTPUT); pinMode(SHCP, OUTPUT); pinMode(STCP, OUTPUT);
}

void loop() {
  // walk a single bit across all 8 outputs (LED chase)
  for (int i = 0; i < 8; i++) {
    writeShift(1 << i);
    delay(100);
  }
}
