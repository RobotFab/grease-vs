// snippet: hcsr04_newping
// tags: distance, ultrasonic, hcsr04, hc-sr04, newping, sonar, cm, inches
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: NewPing
// description: HC-SR04 ultrasonic distance with NewPing — cleaner than manual pulseIn, has timeout.
//   Constructor requires exactly 3 arguments: NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_CM)
//   NOT: NewPing sonar() or NewPing sonar(TRIG, ECHO) — both fail. MAX_CM is required.
//   ping_cm() returns 0 for "no echo", which is out-of-range — NOT zero centimetres. Check it
//   before using the value as a distance, or an obstacle-stop reads 0 cm and slams on the brakes
//   in an empty room.
//   NOT for the Romi 32U4 / Pololu A-Star: on an ATmega32U4 NewPing defines ISR(TIMER4_OVF_vect)
//   (NewPing.cpp), and so does Romi32U4's PololuBuzzer. Linking both fails with
//   "multiple definition of __vector_41" — every Romi sketch that includes NewPing.h, whether or
//   not it uses the timer methods. On a Romi read the HC-SR04 raw: a 10 µs trigger pulse and
//   pulseIn(ECHO, HIGH, timeout), 58 µs per cm. See romi_fsm_manipulator_scan_grab.
// prompts: HC-SR04 NewPing, ultrasonic distance NewPing, sonar NewPing library, distance sensor library
// not-tags: romi32u4, romi
// exact-include: NewPing.h

#include <NewPing.h>

const int TRIGGER = 7;
const int ECHO    = 6;
const int MAX_CM  = 400;

NewPing sonar(TRIGGER, ECHO, MAX_CM);
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;   // max ~20 pings/sec for HC-SR04
  prevMs = now;

  int dist = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
  if (dist == 0) Serial.println("out of range");
  else { Serial.print("distance: "); Serial.print(dist); Serial.println(" cm"); }
}
