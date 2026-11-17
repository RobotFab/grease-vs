// snippet: romi_fsm_pca9685_drive
// tags: romi, romi32u4, pololu, fsm, state-machine, button, pca9685, pwm, motor, servo, i2c, encoder, turn, 90, leonardo
// boards: romi32u4
// library: Romi32U4, Adafruit PWM Servo Driver Library
// description: FSM on Romi 32U4 (detected as Arduino Leonardo) that mixes external PCA9685
//   PWM outputs with the Romi's own wheel drive. State is shown on the LCD.
//   Button A -> step the PWM values on 4 PCA9685 channels (0-3) to the next preset.
//   Button B -> turn the robot ~90 degrees in place using wheel-encoder feedback.
//   WHY PCA9685: the ATmega32U4 has almost no free PWM timers on a Romi -- Timer1 drives the
//     wheels (Romi32U4Motors) and is also what Servo.h would steal; Timer4 runs the buzzer;
//     Timer0 is millis(). The PCA9685 generates all its PWM from its own oscillator over I2C
//     (SDA=pin 2, SCL=pin 3), consuming zero 32U4 timers. Default address 0x40 does not collide
//     with the onboard LSM6DS33 (0x6B) or LIS3MDL (0x1E).
//   90-degree turn geometry: wheel dia ~70 mm, track ~141 mm, so a point turn moves each wheel
//     pi*track/4 ~= 110.7 mm. At 1440 counts/rev over pi*70 mm that is ~725 counts per wheel.
//   NOT: Servo.h / myServo.attach() alongside Romi32U4Motors -- Timer1 conflict stops the wheels
//     silently. Drive aux motors/servos from the PCA9685 instead.
//   NOT: a blocking delay() turn -- the turn runs as a non-blocking FSM state polling encoders.
// prompts: Romi FSM PCA9685, Romi button PWM PCA9685, Romi 4 motor PWM change button, Romi turn 90 degrees button, Romi32U4 PCA9685 drive FSM, Romi state machine servo driver, Romi PCA9685 plus wheel turn, Romi aux motors I2C PWM, Romi encoder turn 90, Romi button A PWM button B turn
// not-tags: esp32, nano33iot, pico
// exact-include: Romi32U4.h, Wire.h, Adafruit_PWMServoDriver.h

#include <Romi32U4.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // Adafruit PWM Servo Driver Library — NOT Adafruit_Servo

Romi32U4Motors   motors;
Romi32U4ButtonA  buttonA;
Romi32U4ButtonB  buttonB;
Romi32U4Encoders encoders;
Romi32U4LCD      lcd;

// PCA9685 at default I2C address 0x40 (SDA=pin 2, SCL=pin 3 on the Romi bus)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

const uint8_t  NUM_PWM_CH = 4;    // drive 4 external motor/servo channels (0..3)
const uint8_t  NUM_STEPS  = 4;    // number of PWM presets to cycle through with button A
uint8_t        pwmStep    = 0;    // index of the current preset

// Preset pulse widths in microseconds for channels 0..3 (ESC/continuous-servo style,
// 1500 us = neutral/stop). Each button-A press advances to the next row.
const uint16_t PWM_US[NUM_STEPS][NUM_PWM_CH] = {
  { 1500, 1500, 1500, 1500 },  // step 0: all neutral
  { 1600, 1600, 1600, 1600 },  // step 1: all slow forward
  { 1700, 1400, 1700, 1400 },  // step 2: alternating
  { 2000, 1000, 2000, 1000 },  // step 3: opposed full scale
};

// Wheel-encoder turn parameters
const int16_t TURN_COUNTS = 725;  // ~90 deg per wheel (see geometry note above)
const int16_t TURN_SPEED  = 150;  // turn-in-place wheel speed

// FSM states
enum State : uint8_t { READY, TURNING };
State state    = READY;
State shownState = TURNING;  // != state so the first frame refreshes the LCD label

// Push the current preset out to the 4 PCA9685 channels
void applyPwmStep() {
  for (uint8_t ch = 0; ch < NUM_PWM_CH; ch++) {
    pwm.writeMicroseconds(ch, PWM_US[pwmStep][ch]);
  }
}

void showState() {
  lcd.clear();
  lcd.gotoXY(0, 0);
  if (state == READY) {
    lcd.print("READY");
    lcd.gotoXY(0, 1);
    lcd.print("PWM ");
    lcd.print(pwmStep);
  } else {
    lcd.print("TURN 90");
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);   // 50 Hz = 20 ms period, standard hobby servo/ESC frequency
  applyPwmStep();       // start every channel at the neutral preset
}

void loop() {
  // Refresh the LCD label whenever the state changes (avoids per-loop flicker)
  if (state != shownState) {
    showState();
    shownState = state;
  }

  switch (state) {
    case READY:
      motors.setSpeeds(0, 0);  // wheels idle while waiting for a command

      if (buttonA.getSingleDebouncedPress()) {
        pwmStep = (pwmStep + 1) % NUM_STEPS;  // advance and wrap
        applyPwmStep();
        showState();  // reflect the new PWM step immediately
      }

      if (buttonB.getSingleDebouncedPress()) {
        // Begin a non-blocking 90-degree turn: zero the encoders, then spin.
        encoders.getCountsAndResetLeft();
        encoders.getCountsAndResetRight();
        state = TURNING;
      }
      break;

    case TURNING:
      // Turn right in place: left wheel forward, right wheel backward.
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);

      // Both wheels travel the same arc length; check the magnitude on either side.
      if (abs(encoders.getCountsLeft())  >= TURN_COUNTS &&
          abs(encoders.getCountsRight()) >= TURN_COUNTS) {
        motors.setSpeeds(0, 0);
        state = READY;
      }
      break;
  }
}
