// snippet: romi_fsm_chord_multimode
// tags: romi, romi32u4, pololu, fsm, state-machine, chord, button, simultaneous, buzzer, note, encoder, drive, turn, pca9685, servo, i2c, lcd, leonardo
// boards: romi32u4
// library: Romi32U4, Adafruit PWM Servo Driver Library
// description: Multi-mode FSM on Romi 32U4 (detected as Arduino Leonardo) driven by button
//   chords. The current mode is shown on the LCD.
//   A + B together -> one buzzer.play() sounding G4 then C5, and print a message over serial.
//   B alone        -> drive forward 2 wheel rotations, then turn ~90 degrees (encoder feedback).
//   C alone        -> move 3 servos on a PCA9685 to preset angles.
//   CHORD DETECTION: getSingleDebouncedPress() consumes each button's edge independently, so it
//     CANNOT distinguish "A+B together" from "B then A". Instead poll isPressed() (a level), wait
//     a short SETTLE window after the first button goes down, then classify the held combination
//     ONCE and latch until all buttons release. This is why "B alone" only fires when A is NOT
//     also held at settle time.
//   Buzzer: Romi32U4Buzzer.play() takes one MML string and plays it in the background (non-
//     blocking). "o4 g o5 c" = G4 then C5. NOT playNote(392,...) chains -- one play() call.
//   90-degree turn: ~725 counts/wheel (wheel dia ~70 mm, track ~141 mm, 1440 counts/rev).
//   2 rotations forward: 2 * 1440 = 2880 counts/wheel.
//   PCA9685: SDA=pin 2, SCL=pin 3, addr 0x40 -- no 32U4 timer used, no clash with the onboard
//     LSM6DS33 (0x6B) or LIS3MDL (0x1E). NOT Servo.h -- it would steal Timer1 from the wheels.
// prompts: Romi button chord FSM, Romi press A and B together buzzer, Romi simultaneous button press, Romi FSM buzzer note G4 C5, Romi drive 2 rotations turn 90, Romi button C PCA9685 3 servos, Romi32U4 multi mode state machine, Romi chord detect isPressed, Romi buzzer serial message button, Romi combined buzzer drive servo FSM
// not-tags: esp32, nano33iot, pico
// exact-include: Romi32U4.h, Wire.h, Adafruit_PWMServoDriver.h

#include <Romi32U4.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Romi32U4Motors   motors;
Romi32U4ButtonA  buttonA;
Romi32U4ButtonB  buttonB;
Romi32U4ButtonC  buttonC;
Romi32U4Buzzer   buzzer;
Romi32U4Encoders encoders;
Romi32U4LCD      lcd;

// PCA9685 for the 3-servo action (SDA=pin 2, SCL=pin 3, default address 0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
const uint8_t  SERVO_CH[3]   = { 0, 1, 2 };      // three servo channels
const uint16_t SERVO_POSE[3] = { 1200, 1500, 1800 };  // preset pulse widths (us)

// Encoder targets
const int16_t FWD_COUNTS  = 2880;  // 2 wheel rotations (2 * 1440 counts/rev)
const int16_t TURN_COUNTS = 725;   // ~90 deg per wheel
const int16_t DRIVE_SPEED = 150;
const int16_t TURN_SPEED  = 150;

// Chord classifier
const unsigned long SETTLE_MS = 50;  // let the button combination settle before classifying
bool          pressLatched = false;  // true once the current press has been classified
unsigned long pressStart   = 0;

// Non-blocking "B alone" drive sequence: forward 2 rev, then turn 90
enum Task : uint8_t { TASK_NONE, TASK_FORWARD, TASK_TURN };
Task task = TASK_NONE;

// FSM mode purely for the LCD label
enum Mode : uint8_t { MODE_IDLE, MODE_CHORD, MODE_DRIVE, MODE_SERVO };
Mode mode = MODE_IDLE;
Mode shownMode = MODE_CHORD;  // != mode so the first frame refreshes the LCD

void showMode() {
  lcd.clear();
  lcd.gotoXY(0, 0);
  switch (mode) {
    case MODE_IDLE:  lcd.print("IDLE");     break;
    case MODE_CHORD: lcd.print("A+B TONE");  break;
    case MODE_DRIVE: lcd.print("B DRIVE");   break;
    case MODE_SERVO: lcd.print("C SERVO");   break;
  }
}

void startServos() {
  for (uint8_t i = 0; i < 3; i++) {
    pwm.writeMicroseconds(SERVO_CH[i], SERVO_POSE[i]);
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz hobby-servo frequency
}

void loop() {
  unsigned long now = millis();

  // ================= Drive task (owns the wheels while active) =================
  if (task != TASK_NONE) {
    switch (task) {
      case TASK_FORWARD:
        motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);
        if (abs(encoders.getCountsLeft())  >= FWD_COUNTS &&
            abs(encoders.getCountsRight()) >= FWD_COUNTS) {
          encoders.getCountsAndResetLeft();
          encoders.getCountsAndResetRight();
          task = TASK_TURN;  // chain into the 90-degree turn
        }
        break;
      case TASK_TURN:
        motors.setSpeeds(TURN_SPEED, -TURN_SPEED);  // spin right in place
        if (abs(encoders.getCountsLeft())  >= TURN_COUNTS &&
            abs(encoders.getCountsRight()) >= TURN_COUNTS) {
          motors.setSpeeds(0, 0);
          task = TASK_NONE;
          mode = MODE_IDLE;
        }
        break;
      default: break;
    }
    // While a drive task runs, ignore new button input.
    if (mode != shownMode) { showMode(); shownMode = mode; }
    return;
  }

  motors.setSpeeds(0, 0);  // wheels idle when no drive task is running

  // ================= Chord classifier (level-based, latched) =================
  bool a = buttonA.isPressed();
  bool b = buttonB.isPressed();
  bool c = buttonC.isPressed();
  bool anyDown = a || b || c;

  if (!anyDown) {
    pressLatched = false;  // all released -> ready for the next press
  } else {
    if (!pressLatched && pressStart == 0) {
      pressStart = now;    // a fresh press just began; start the settle timer
    }
    if (!pressLatched && (now - pressStart >= SETTLE_MS)) {
      // Classify the settled combination exactly once.
      if (a && b) {
        mode = MODE_CHORD;
        buzzer.play("o4 g o5 c");        // one call: G4 then C5, non-blocking
        Serial.println("CHORD A+B: G4 + C5");
      } else if (b && !a && !c) {
        mode = MODE_DRIVE;
        encoders.getCountsAndResetLeft();
        encoders.getCountsAndResetRight();
        task = TASK_FORWARD;             // kick off forward-2-rev-then-turn
      } else if (c && !a && !b) {
        mode = MODE_SERVO;
        startServos();
      }
      pressLatched = true;
    }
  }
  if (!anyDown) pressStart = 0;  // reset the settle timer once buttons are released

  // LCD reflects the latest mode
  if (mode != shownMode) { showMode(); shownMode = mode; }
}
