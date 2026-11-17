// snippet: decl_practice_144
// tags: globals, declare, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Declare const int TRIG_PIN=7, ECHO_PIN=8, LED_PIN=13, MOTOR_PIN=5 at global scope
// prompts: declare, const, trig_pin=7, echo_pin=8, led_pin=13, motor_pin=5
// verified: compile-only
// source: kd-generated (id 144)

const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
const int LED_PIN = 13;
const int MOTOR_PIN = 5;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);  // returns echo duration in us; divide by 58 for cm (speed of sound)
  float distance = duration * 0.034 / 2;

  if (distance < 20) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(MOTOR_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }
}
