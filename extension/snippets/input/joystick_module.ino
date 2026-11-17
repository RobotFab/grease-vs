// snippet: joystick_module
// tags: joystick, analog, input, xy, button, axis
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read a two-axis analog joystick module (VRx, VRy) plus the built-in push button.
//   Wiring: VRx → X_PIN, VRy → Y_PIN, SW → BTN_PIN, VCC → 5V, GND → GND.
//   Center position returns ~512. Button is active LOW (pressed = 0).
// prompts: joystick module, analog joystick input, XY axis joystick, two-axis analog stick, game controller joystick
//
// ---- CONFIGURE ----
#define X_PIN    A0
#define Y_PIN    A1
#define BTN_PIN  2    // digital pin; joystick button pulls LOW when pressed
// -------------------

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop() {
  int x   = analogRead(X_PIN);
  int y   = analogRead(Y_PIN);
  bool btn = !digitalRead(BTN_PIN);   // LOW when pressed

  Serial.print("X: "); Serial.print(x);
  Serial.print("  Y: "); Serial.print(y);
  Serial.print("  BTN: "); Serial.println(btn ? "PRESSED" : "released");
  delay(100);
}
