// snippet: keypad_4x4
// tags: keypad, matrix, 4x4, input, buttons, keypad, scan
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Keypad
// description: Read a 4×4 matrix keypad (16 keys, 8 wires) using the Keypad library.
//   Connect row pins to ROW_PINS and column pins to COL_PINS — all configurable below.
//   No external pull-ups needed; the library enables internal pull-ups.
// prompts: 4x4 keypad matrix, 16-key input, phone keypad, number pad, matrix keypad scan, Keypad library
//
// ---- CONFIGURE ----
// exact-include: Keypad.h
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {9, 8, 7, 6};   // R1..R4
byte colPins[COLS] = {5, 4, 3, 2};   // C1..C4
// -------------------

#include <Keypad.h>

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  Serial.println("Keypad ready");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
  }
}
