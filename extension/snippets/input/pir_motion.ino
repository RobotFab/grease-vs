// snippet: pir_motion
// tags: pir, motion, passive infrared, hcsr501, detect, digital, interrupt
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Detect motion using a PIR sensor (HC-SR501 or equivalent).
// prompts: PIR motion sensor, motion detection, HC-SR501, passive infrared, detect presence, motion alarm, motion triggered
//   Output pin goes HIGH for ~3s when motion is detected (adjustable via onboard trimmers).
//   Wiring: OUT → PIR_PIN, VCC → 5V, GND → GND.
//   Allow 30–60s warm-up time after power-on before readings are reliable.
//
// ---- CONFIGURE ----
#define PIR_PIN   3
#define LED_PIN  13
// -------------------

volatile bool motionFlag = false;

void onMotion() { motionFlag = true; }

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), onMotion, RISING);
  Serial.println("PIR ready — allow 30s warm-up");
}

void loop() {
  if (motionFlag) {
    motionFlag = false;
    Serial.print("Motion detected at ");
    Serial.print(millis() / 1000.0, 1);
    Serial.println("s");
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }
}
