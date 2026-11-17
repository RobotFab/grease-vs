// snippet: romi_fsm_mode_select_imu
// tags: romi, romi32u4, pololu, fsm, state-machine, mode-select, button, imu, lsm6, lsm6ds3, roll, pitch, serial-plotter, lcd, motor, drive, leonardo
// boards: romi32u4
// library: Romi32U4, LSM6
// description: Mode-select FSM on Romi 32U4 (detected as Arduino Leonardo). Three onboard
//   buttons pick the active mode; the current mode is shown on the LCD.
//   Button A -> ROLL:  read and display the roll angle (readout on LCD + Serial Monitor).
//   Button B -> PITCH: stream the pitch angle for the Arduino Serial Plotter ("Pitch:<val>").
//   Button C -> DRIVE: drive both wheels forward at speed 150.
//   Romi32U4 API:
//   Romi32U4Motors motors  -> motors.setSpeeds(left, right);  range -300..300 (allowTurbo -> +-400)
//   Romi32U4ButtonA/B/C    -> getSingleDebouncedPress() -> bool (edge detect, self-debouncing)
//   Romi32U4LCD lcd        -> lcd.clear(); lcd.gotoXY(col, row); lcd.print(...);  8x2 characters
//   IMU: onboard LSM6DS33 via the LSM6 library (NOT Arduino_LSM6DS3 -- that is Nano 33 IoT only).
//     imu.a.x/y/z are int16_t; /16384.0 -> g.  imu.g.x/y/z are int16_t; /114.3 -> deg/s (245 dps).
//   SAFETY: motors run ONLY in DRIVE. Every other mode calls setSpeeds(0,0) so a mode switch
//     out of DRIVE stops the wheels immediately.
//   Buttons are read with getSingleDebouncedPress() because each mode is chosen by a SINGLE
//     button -- no simultaneous-press (chord) detection is needed here.
//   Serial Plotter: print "Pitch:<value>" (one labeled series per line). The plotter graphs it;
//     the Serial Monitor still shows it as text.  NOT: printing extra text in PITCH mode -- it
//     corrupts the plot. Keep DRIVE/ROLL chatter out of the PITCH stream.
// prompts: Romi mode select FSM, Romi button A B C modes, Romi FSM IMU roll pitch, Romi state machine buttons, Romi read roll button, Romi plot pitch serial plotter, Romi drive forward button, Romi32U4 FSM accelerometer, Romi button pick mode LCD, Romi IMU FSM drive
// not-tags: nano33iot, arduino-lsm6ds3, mpu6050, esp32, pico
// exact-include: Romi32U4.h, Wire.h, LSM6.h

#include <Romi32U4.h>
#include <Wire.h>
#include <LSM6.h>

Romi32U4Motors  motors;
Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;
Romi32U4LCD     lcd;
LSM6            imu;

// FSM modes selected by the three onboard buttons
enum Mode : uint8_t { MODE_ROLL, MODE_PITCH, MODE_DRIVE };
Mode mode = MODE_ROLL;      // start in a motors-off readout mode
Mode shownMode = MODE_DRIVE; // force a first-frame LCD label refresh (any value != mode)

const int16_t DRIVE_SPEED = 150;  // forward wheel speed in DRIVE mode

// Complementary filter state (blend gyro integration with accel tilt)
const float ALPHA = 0.98f;  // 98% gyro, 2% accel correction
float roll  = 0.0f;         // deg
float pitch = 0.0f;         // deg
unsigned long prevMs = 0;
unsigned long lcdMs  = 0;   // LCD refresh rate limiter
unsigned long plotMs = 0;   // Serial Plotter stream rate limiter

// Print the fixed mode label on LCD row 0 and clear the value row.
void showModeLabel(Mode m) {
  lcd.clear();
  lcd.gotoXY(0, 0);
  switch (m) {
    case MODE_ROLL:  lcd.print("A: ROLL");  break;
    case MODE_PITCH: lcd.print("B: PITCH"); break;
    case MODE_DRIVE: lcd.print("C: DRIVE"); break;
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!imu.init()) {
    lcd.clear();
    lcd.print("IMU FAIL");
    Serial.println("IMU init failed");
    while (1) { }
  }
  imu.enableDefault();
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();

  // ---- IMU update every loop so the filter has a steady dt ----
  float dt = (now - prevMs) * 0.001f;  // seconds
  prevMs = now;
  imu.read();
  float ax = imu.a.x / 16384.0f;
  float ay = imu.a.y / 16384.0f;
  float az = imu.a.z / 16384.0f;
  float gx = imu.g.x / 114.3f;  // deg/s
  float gy = imu.g.y / 114.3f;  // deg/s
  float accelRoll  = atan2f(ay, az) * 180.0f / M_PI;
  float accelPitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / M_PI;
  roll  = ALPHA * (roll  + gx * dt) + (1.0f - ALPHA) * accelRoll;
  pitch = ALPHA * (pitch + gy * dt) + (1.0f - ALPHA) * accelPitch;

  // ---- Mode selection: one button per mode ----
  if (buttonA.getSingleDebouncedPress()) mode = MODE_ROLL;
  if (buttonB.getSingleDebouncedPress()) mode = MODE_PITCH;
  if (buttonC.getSingleDebouncedPress()) mode = MODE_DRIVE;

  // Refresh the LCD label only when the mode actually changes (avoids flicker)
  if (mode != shownMode) {
    showModeLabel(mode);
    shownMode = mode;
  }

  // ---- Act on the current mode ----
  switch (mode) {
    case MODE_ROLL:
      motors.setSpeeds(0, 0);  // readout mode: wheels stopped
      if (now - lcdMs >= 150) {
        lcdMs = now;
        lcd.gotoXY(0, 1);
        lcd.print(roll, 1);
        lcd.print("    ");  // pad to overwrite stale characters
        Serial.print("Roll: "); Serial.println(roll, 2);
      }
      break;

    case MODE_PITCH:
      motors.setSpeeds(0, 0);  // readout mode: wheels stopped
      // Stream ONLY the pitch value for the Serial Plotter (labeled single series)
      if (now - plotMs >= 50) {
        plotMs = now;
        Serial.print("Pitch:"); Serial.println(pitch, 2);
      }
      if (now - lcdMs >= 150) {
        lcdMs = now;
        lcd.gotoXY(0, 1);
        lcd.print(pitch, 1);
        lcd.print("    ");
      }
      break;

    case MODE_DRIVE:
      motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);  // both wheels forward at 150
      if (now - lcdMs >= 150) {
        lcdMs = now;
        lcd.gotoXY(0, 1);
        lcd.print("spd ");
        lcd.print(DRIVE_SPEED);
        lcd.print(" ");
      }
      break;
  }
}
