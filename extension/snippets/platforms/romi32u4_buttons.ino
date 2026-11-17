// snippet: romi32u4_buttons
// tags: romi, pololu, romi32u4, button, buttons, press, debounce, input
// boards: romi32u4
// library: Romi32U4
// description: Read the Romi 32U4 onboard buttons A, B, C with debouncing.
//   Buttons are CLASSES: Romi32U4ButtonA / Romi32U4ButtonB / Romi32U4ButtonC.
//   Methods: getSingleDebouncedPress() (true once per press) and isPressed() (level).
//   NOT: buttonA.read(), digitalRead(BUTTON_A) — no such API; use the button classes.
// prompts: Romi button A B C, Romi32U4 button press, detect button press Romi, Romi getSingleDebouncedPress, Romi button isPressed, wait for button Romi, when I press the button Romi
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (buttonA.getSingleDebouncedPress()) {
    Serial.println("A pressed");
  }
  if (buttonB.getSingleDebouncedPress()) {
    Serial.println("B pressed");
  }
  if (buttonC.isPressed()) {
    Serial.println("C held");
  }
}
