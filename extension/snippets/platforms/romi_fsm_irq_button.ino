// snippet: romi_fsm_irq_button
// tags: romi, romi32u4, fsm, state-machine, interrupt, irq, button, motor, lcd, leonardo
// boards: romi32u4
// library: Romi32U4
// description: Finite State Machine on Romi 32U4 (detected as Arduino Leonardo).
//   States: IDLE → (external IRQ switch falls on pin 7) → RUNNING → (button A press) → HALTED → (button A press) → IDLE.
//   Romi32U4 API:
//   Romi32U4Motors motors  → motors.setSpeeds(leftSpeed, rightSpeed);  range −300..300
//     (library clamps to ±300; motors.allowTurbo(true) raises the limit to ±400)
//   Romi32U4ButtonA btnA   → btnA.getSingleDebouncedPress() → bool (edge detect, self-debouncing)
//   Romi32U4LCD lcd        → lcd.clear(); lcd.gotoXY(col, row); lcd.print("text");  8×2 characters
//   Pin 7 = INT6 on ATmega32U4 (the first interrupt-capable pin not used by Romi hardware).
//   enum State : uint8_t → 1-byte atomic write on AVR, safe to write from ISR without cli/sei.
// prompts: romi state machine, romi FSM, romi IRQ switch state transition, romi finite state machine interrupt, robot FSM bump switch, Romi32U4 FSM, romi32u4 state machine interrupt button
// exact-include: Romi32U4.h

#include <Romi32U4.h>

// External limit/bump switch: normally-open, wired between SWITCH_PIN and GND.
// Fires on falling edge (INPUT_PULLUP → switch closes → LOW).
// Pin 7 = INT6 on ATmega32U4 (Leonardo / Romi 32U4); safe to use alongside Romi hardware.
const uint8_t SWITCH_PIN = 7;

enum State : uint8_t { IDLE, RUNNING, HALTED };
volatile State state = IDLE;  // uint8_t: 1-byte write is atomic on AVR

Romi32U4Motors  motors;
Romi32U4ButtonA btnA;
Romi32U4LCD     lcd;

void onSwitch() {
  if (state == IDLE) state = RUNNING;  // atomic on AVR — no cli/sei required
}

void showState(const char* label) {
  lcd.clear();
  lcd.gotoXY(0, 0);
  lcd.print(label);
  Serial.println(label);
}

void setup() {
  Serial.begin(9600);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), onSwitch, FALLING);
  showState("IDLE");
}

void loop() {
  switch (state) {
    case IDLE:
      // Motors off; await falling edge on SWITCH_PIN (ISR drives transition)
      motors.setSpeeds(0, 0);
      break;

    case RUNNING:
      // Drive forward; button A press → HALTED
      motors.setSpeeds(150, 150);
      if (btnA.getSingleDebouncedPress()) {
        motors.setSpeeds(0, 0);
        state = HALTED;
        showState("HALTED");
      }
      break;

    case HALTED:
      // Stopped; button A press → back to IDLE
      motors.setSpeeds(0, 0);
      if (btnA.getSingleDebouncedPress()) {
        state = IDLE;
        showState("IDLE");
      }
      break;
  }
}
