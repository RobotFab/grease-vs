// snippet: motor_dc_9
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Motor direction toggle on button press (pin 2 INPUT_PULLUP, debounce 50ms)
// prompts: motor, direction, toggle, button, press, input_pullup
// verified: compile-only
// source: kd-generated (id 9)

const int BTN_PIN = 2;
const int MOTOR_PIN = 5;
const unsigned long DEBOUNCE_MS = 50;

int lastReading = HIGH;
int btnState = HIGH;
unsigned long lastChangeMs = 0;
bool motorForward = true;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int reading = digitalRead(BTN_PIN);
  if (reading != lastReading) lastChangeMs = millis();
  if (millis() - lastChangeMs > DEBOUNCE_MS) {
    if (reading != btnState) {
      btnState = reading;
      if (btnState == LOW) {  // pressed (active-low with pull-up)
        motorForward = !motorForward;
        digitalWrite(MOTOR_PIN, motorForward ? HIGH : LOW);
        Serial.println(motorForward ? "FWD" : "REV");
      }
    }
  }
  lastReading = reading;
}
