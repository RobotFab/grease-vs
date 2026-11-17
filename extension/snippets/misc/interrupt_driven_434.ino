// snippet: interrupt_driven_434
// tags: avr/io, avr/interrupt
// boards: avr
// library: avr/io, avr/interrupt
// description: Read a 4×4 keypad using pin-change interrupts (PCINT) instead of polling
// prompts: keypad, pin-change, interrupts, (pcint), instead, polling
// verified: compile-only
// source: kd-generated (id 434)
// exact-include: avr/io.h, avr/interrupt.h

#include <avr/io.h>
#include <avr/interrupt.h>

#define ROWS 4
#define COLS 4
const byte rowPins[ROWS] = {9, 8, 7, 6};   // R1..R4
const byte colPins[COLS] = {5, 4, 3, 2};   // C1..C4

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

volatile bool keyEvent = false;   // set by the ISR, consumed in loop()

void setup() {
  Serial.begin(115200);
  Serial.println("Keypad ready");

  // Set row pins as inputs with pull-ups
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }

  // Set column pins as outputs, idle HIGH (pressed row reads LOW)
  for (int i = 0; i < COLS; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }

  // Enable pin-change interrupts on the four row pins (PCINT0..3 / PCIE0 bank)
  sei();
  PCICR  |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);
}

void loop() {
  // The matrix scan and Serial print run here, NOT in the ISR: an interrupt
  // handler must return fast, and Serial is slow / itself interrupt-driven, so
  // printing inside the ISR would hang. The ISR only flags that a row changed.
  if (keyEvent) {
    keyEvent = false;
    for (int col = 0; col < COLS; col++) {
      digitalWrite(colPins[col], LOW);       // drive this column low
      for (int row = 0; row < ROWS; row++) {
        if (digitalRead(rowPins[row]) == LOW) {  // pressed key pulls the row low
          Serial.print("Key pressed: ");
          Serial.println(keys[row][col]);
        }
      }
      digitalWrite(colPins[col], HIGH);      // restore idle
    }
  }
}

// Pin-change interrupt service routine — keep it tiny.
ISR(PCINT0_vect) {
  keyEvent = true;
}
