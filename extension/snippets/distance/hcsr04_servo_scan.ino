// snippet: hcsr04_servo_scan
// tags: hcsr04, ultrasonic, servo, scan, distance, sweep, radar, nonblocking
// boards: avr, megaavr, nano33iot, rp2040
// library: NewPing, Servo
// description: Mount an HC-SR04 on a servo to sweep 0-180 degrees and print angle,distance pairs
//   as CSV — useful for simple 2D obstacle mapping or a radar display.
//   Wiring: TRIG -> TRIG_PIN, ECHO -> ECHO_PIN, servo signal -> SERVO_PIN.
//   The sweep is stepped from loop() on a millis() timer, one step per pass. A blocking
//   `for (angle...) { servo.write(angle); delay(50); }` sweep takes ~4 s to come back round,
//   and for those 4 s the sketch cannot read a bumper, a button, or a serial stop command —
//   the robot is blind and deaf while its radar spins.
//   ping_cm() returns 0 for "no echo" (out of range), NOT zero centimetres — check before use.
//   Servo.h on AVR takes over Timer1, so PWM on pins 9 and 10 stops working while a servo is
//   attached. Keep motor PWM off those two pins on an Uno/Nano.
//   NOT for the Romi 32U4 / Pololu A-Star, for three separate reasons:
//     - Servo.h claims Timer1, which is where Romi32U4Motors generates the wheel PWM;
//     - NewPing defines ISR(TIMER4_OVF_vect) on 32U4, and so does Romi32U4's PololuBuzzer —
//       "multiple definition of __vector_41" at link time;
//     - pins 9 and 10 ARE the Romi's right/left motor PWM outputs, and pin 6 is its buzzer.
//   On a Romi: PCA9685 for the servo, raw pulseIn() for the HC-SR04 — see
//   romi_fsm_manipulator_scan_grab.
// prompts: ultrasonic radar, servo scan distance, HC-SR04 servo sweep, 2D obstacle scan, sonar radar, rotating ultrasonic
// not-tags: romi32u4, romi, esp32
// exact-include: NewPing.h, Servo.h

#include <NewPing.h>
#include <Servo.h>

// ---- CONFIGURE ----
#define TRIG_PIN   9
#define ECHO_PIN  10
#define SERVO_PIN  6
#define MAX_DIST  200   // cm
#define STEP_DEG    5   // degrees per step
#define STEP_MS    50   // settle time between steps — a hobby servo needs ~40 ms for 5 degrees
// -------------------

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);
Servo   scanServo;

int angle   = 0;
int stepDir = STEP_DEG;   // flips sign at each end of the sweep
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  scanServo.attach(SERVO_PIN);
  scanServo.write(angle);
  Serial.println("angle,distance_cm");
}

void loop() {
  // One step per interval. Everything else in loop() keeps running between steps.
  if (millis() - prevMs < STEP_MS) return;
  prevMs = millis();

  int dist = sonar.ping_cm();   // 0 = no echo (out of range), not 0 cm
  Serial.print(angle);
  Serial.print(',');
  Serial.println(dist);         // a plotter/mapper should discard rows where distance == 0

  angle += stepDir;
  if (angle >= 180) { angle = 180; stepDir = -STEP_DEG; }
  if (angle <= 0)   { angle = 0;   stepDir =  STEP_DEG; }
  scanServo.write(angle);
}
