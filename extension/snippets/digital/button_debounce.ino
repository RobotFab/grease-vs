// snippet: button_debounce
// tags: button, debounce, digital, input, pullup, press, edge, state
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// description: Debounce a push button with millis() — detects press edges reliably, no delay().
//   NOT for the Romi 32U4: pin 2 is its I2C SDA line, and the board already has three
//   debounced buttons — use Romi32U4ButtonA and getSingleDebouncedPress() there.
// prompts: button debounce, push button, debounce press, detect button press, INPUT_PULLUP button, button edge detect, reliable button, tactile switch
// not-tags: romi32u4, romi

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

const int BTN_PIN = 2;
const int LED_PIN = LED_BUILTIN;
const unsigned long DEBOUNCE_MS = 50;

int lastReading   = HIGH;
int btnState      = HIGH;
unsigned long lastChangeMs = 0;
int ledState = LOW;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int reading = digitalRead(BTN_PIN);
  if (reading != lastReading) lastChangeMs = millis();
  if (millis() - lastChangeMs > DEBOUNCE_MS) {
    if (reading != btnState) {
      btnState = reading;
      if (btnState == LOW) {  // pressed (active-low with pull-up)
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        Serial.println("pressed");
      }
    }
  }
  lastReading = reading;
}
