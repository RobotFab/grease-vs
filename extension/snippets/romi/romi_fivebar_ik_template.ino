// snippet: romi_fivebar_ik_template
// tags: romi32u4, five-bar, fivebar, four-bar, linkage, arm, inverse-kinematics, ik, forward-kinematics, servo, pca9685, rbe2001, shelf, teleop, straight-line, plotter
// boards: romi32u4
// library: wpi-32u4-library, Adafruit PWM Servo Driver Library
// description: RBE 2001 arm template. The five-bar IK runs ON the Romi 32U4: reach check, two-point servo calibration, straight-line tip moves, XY Plot Pairs.
//   Convention (HW4): O=(0,0) holds servo 1 (theta2), C=(OC,0) holds servo 2 (theta4), angles CCW from +x, the tip P joins AP and PB.
//   Serial 115200: "m X Y" moves the tip to (X,Y) mm in a straight line; i/k/j/l jog 5 mm; w/a/s/d drive; x stops; h homes; p prints.
//   NOT: Servo.h on the 32U4 (it takes Timer1, which also drives the wheels).
// prompts: romi five bar ik, romi arm inverse kinematics on board, rbe2001 arm template, five-bar servo ik romi, pca9685 romi arm, romi shelf cube arm, proof of ik straight line
// not-tags: esp32, rp2040
// exact-include: Wire.h, Adafruit_PWMServoDriver.h, Chassis.h

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Chassis.h>

// ---- GEOMETRY: measure YOUR arm pin center to pin center, in mm ----
const float OC = 40.0;   // O to C: the two servo shafts, both on the x axis (ground link).
const float OA = 100.0;  // Crank on servo 1 (input link).
const float BC = 100.0;  // Crank on servo 2 (input link).
const float AP = 90.0;   // A to the tip P (coupler).
const float PB = 90.0;   // B to the tip P (coupler).
const float ELBOW_A = +1.0;  // +1 puts A left of the line O->P, -1 right of it (assembly mode).
const float ELBOW_B = -1.0;  // Opposite sign to ELBOW_A keeps both elbows outside.
const float MAX_COS = 0.995; // |cos| above 0.995 is within 5.7 deg of a straight chain (singular pose).

// ---- SERVO MAP: two measured points per servo, arm angle (deg) -> pulse (us) ----
// Send a pulse, measure the crank with a protractor, repeat at a second angle; the gear ratio is inside the slope.
struct ServoMap {
  uint8_t ch;          // PCA9685 channel.
  float degA, usA;     // First measured point.
  float degB, usB;     // Second measured point.
  float degMin, degMax;  // Mechanical limits of the crank, measured, not guessed.
};
ServoMap S1 = {0, 90.0, 1500, 180.0, 2500, 45.0, 225.0};  // Servo 1 turns theta2.
ServoMap S2 = {1, 90.0, 1500, 0.0, 500, -45.0, 135.0};    // Servo 2 turns theta4.
const float US_MIN = 500, US_MAX = 2500;  // Pulses outside 500-2500 us drive most servos into their stops.

// ---- MOTION ----
const float HOME_X = 20.0, HOME_Y = 150.0;  // A pose inside the workspace for your geometry.
const float JOG_MM = 5.0;           // One i/j/k/l key moves the tip 5 mm.
const float STEP_MM = 2.0;          // 2 mm per 20 ms frame = 100 mm/s along a straight line (Proof of IK).
const uint16_t FRAME_MS = 20;       // The loop never blocks, so the wheels and the arm move together.
const uint16_t PRINT_MS = 100;      // 10 lines per second for the Plotter.
const float DRIVE_CMPS = 15.0;      // w/s drive speed, cm/s.
const float TURN_DPS = 60.0;        // a/d turn rate, deg/s.
const uint16_t DRIVE_HOLD_MS = 300; // No drive key for 300 ms stops the wheels (dead-man key).

Adafruit_PWMServoDriver pca(0x40);  // 0x40 is the PCA9685 address with no solder jumpers.
Chassis chassis;                     // Course defaults: 7 cm wheels, 1440 ticks/rev, 14.7 cm track.

float curX, curY;        // Where the tip is now (the last pose that solved).
float tgtX, tgtY;        // Where the tip is going.
float curT2, curT4;      // Crank angles of the current pose, deg.
unsigned long lastIkUs;  // Solve time of the last IK call, us.
unsigned long lastFrame, lastPrint, lastDriveKey;
bool driving = false;
char line[24];           // "m X Y" needs fewer than 24 characters.
uint8_t lineLen = 0;

// Inverse kinematics: tip (x, y) in mm -> crank angles in deg. Returns false when the tip is out of reach.
bool solveIK(float x, float y, float &t2, float &t4) {
  float r1 = sqrt(x * x + y * y);              // O to P.
  float r2 = sqrt((x - OC) * (x - OC) + y * y); // C to P.
  if (r1 < 1.0 || r2 < 1.0) return false;      // The tip on top of a shaft has no unique solution.
  float c1 = (OA * OA + r1 * r1 - AP * AP) / (2.0 * OA * r1);  // Law of cosines at O.
  float c2 = (BC * BC + r2 * r2 - PB * PB) / (2.0 * BC * r2);  // Law of cosines at C.
  if (fabs(c1) > MAX_COS || fabs(c2) > MAX_COS) return false;  // acos() needs -1..1; beyond that the links cannot reach.
  t2 = degrees(atan2(y, x) + ELBOW_A * acos(c1));
  t4 = degrees(atan2(y, x - OC) + ELBOW_B * acos(c2));
  return true;
}

// Forward kinematics: crank angles -> tip, by intersecting two circles (used to check the IK).
bool solveFK(float t2, float t4, float &x, float &y) {
  float ax = OA * cos(radians(t2)), ay = OA * sin(radians(t2));
  float bx = OC + BC * cos(radians(t4)), by = BC * sin(radians(t4));
  float dx = bx - ax, dy = by - ay, d = sqrt(dx * dx + dy * dy);
  if (d < 1e-3 || d > AP + PB || d < fabs(AP - PB)) return false;  // The couplers cannot close the loop.
  float a = (AP * AP - PB * PB + d * d) / (2.0 * d);  // Distance from A to the chord through P.
  float h = sqrt(max(0.0, AP * AP - a * a));          // Height of P above the line A->B.
  x = ax + a * dx / d - h * dy / d;  // The minus/plus pair picks P on the left of A->B (tip above).
  y = ay + a * dy / d + h * dx / d;
  return true;
}

// Arm angle -> pulse through the two measured points (linear map, so the gear ratio is included).
float pulseFor(const ServoMap &s, float deg) {
  return s.usA + (deg - s.degA) * (s.usB - s.usA) / (s.degB - s.degA);
}

// Checks every limit BEFORE a pulse reaches a servo. Returns NULL when the pose is safe, else the reason.
const __FlashStringHelper *poseProblem(float t2, float t4) {
  if (t2 < S1.degMin || t2 > S1.degMax) return F("theta2 outside its measured limits");
  if (t4 < S2.degMin || t4 > S2.degMax) return F("theta4 outside its measured limits");
  float u1 = pulseFor(S1, t2), u2 = pulseFor(S2, t4);
  if (u1 < US_MIN || u1 > US_MAX || u2 < US_MIN || u2 > US_MAX) return F("pulse outside 500-2500 us");
  return NULL;
}

// Solves, checks, and only then moves. A rejected target leaves the arm where it is.
// Why reject and not clamp? A clamped target moves the arm to a point you never asked for.
bool moveTipNow(float x, float y) {
  float t2, t4;
  unsigned long t0 = micros();
  bool ok = solveIK(x, y, t2, t4);
  lastIkUs = micros() - t0;  // About 1-2 ms on the 32U4: it has no FPU, so float math runs in software.
  if (!ok) return false;
  if (poseProblem(t2, t4) != NULL) return false;
  pca.writeMicroseconds(S1.ch, (uint16_t)pulseFor(S1, t2));
  pca.writeMicroseconds(S2.ch, (uint16_t)pulseFor(S2, t4));
  curX = x; curY = y; curT2 = t2; curT4 = t4;  // Update the pose only after the move was accepted.
  return true;
}

// Explains a rejected target once, so you know WHY the arm did not move.
void reportReject(float x, float y) {
  float t2, t4;
  Serial.print(F("IK: rejected ("));
  Serial.print(x, 1); Serial.print(F(", ")); Serial.print(y, 1); Serial.print(F(") mm: "));
  if (!solveIK(x, y, t2, t4)) Serial.println(F("out of reach or near a singular pose"));
  else Serial.println(poseProblem(t2, t4));
}

// One 20 ms frame: step the tip toward the target along a straight line.
void stepArm() {
  float dx = tgtX - curX, dy = tgtY - curY;
  float d = sqrt(dx * dx + dy * dy);
  if (d < 0.01) return;  // Already there.
  float nx = (d <= STEP_MM) ? tgtX : curX + dx / d * STEP_MM;
  float ny = (d <= STEP_MM) ? tgtY : curY + dy / d * STEP_MM;
  if (!moveTipNow(nx, ny)) {  // The line leaves the workspace: stop at the last good point.
    reportReject(nx, ny);
    tgtX = curX; tgtY = curY;
  }
}

void setTarget(float x, float y) {
  float t2, t4;
  if (!solveIK(x, y, t2, t4) || poseProblem(t2, t4) != NULL) {  // Check the end point before the first step.
    reportReject(x, y);
    return;
  }
  tgtX = x; tgtY = y;
}

// label:value pairs; tip_x + tip_y form one Plot Pair, so the Plotter draws the tip path in XY.
void printPose() {
  float fx, fy;
  solveFK(curT2, curT4, fx, fy);  // FK of the angles you sent: it must land on tip_x/tip_y.
  Serial.print(F("tip_x:")); Serial.print(curX, 1);
  Serial.print(F(",tip_y:")); Serial.print(curY, 1);
  Serial.print(F(",t2:")); Serial.print(curT2, 1);
  Serial.print(F(",t4:")); Serial.print(curT4, 1);
  Serial.print(F(",fk_err_mm:")); Serial.print(sqrt(sq(fx - curX) + sq(fy - curY)), 2);
  Serial.print(F(",ik_us:")); Serial.println(lastIkUs);
}

void drive(float cmps, float dps) {
  chassis.setTwist(cmps, dps);
  driving = true;
  lastDriveKey = millis();
}

// Single keys, as most of you used them: one character, no Enter needed.
void runKey(char c) {
  switch (c) {
    case 'i': setTarget(tgtX, tgtY + JOG_MM); break;
    case 'k': setTarget(tgtX, tgtY - JOG_MM); break;
    case 'j': setTarget(tgtX - JOG_MM, tgtY); break;
    case 'l': setTarget(tgtX + JOG_MM, tgtY); break;
    case 'w': drive(DRIVE_CMPS, 0); break;
    case 's': drive(-DRIVE_CMPS, 0); break;
    case 'a': drive(0, TURN_DPS); break;
    case 'd': drive(0, -TURN_DPS); break;
    case 'x': chassis.idle(); driving = false; tgtX = curX; tgtY = curY; break;  // x stops the wheels and the arm.
    case 'h': setTarget(HOME_X, HOME_Y); break;
    case 'p': printPose(); break;
    default: break;  // Newlines and unknown keys do nothing.
  }
}

// "m X Y": strtod() instead of sscanf("%f"), because the AVR sscanf has no float support.
void runLine(char *s) {
  char *p = s + 1;
  char *end;
  float x = strtod(p, &end);
  if (end == p) { Serial.println(F("Use: m X Y   (mm, for example m 20 150)")); return; }
  p = end;
  float y = strtod(p, &end);
  if (end == p) { Serial.println(F("Use: m X Y   (mm, for example m 20 150)")); return; }
  setTarget(x, y);
}

// Non-blocking reader: "m ..." is buffered until Enter, every other character is a key.
void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (lineLen > 0 || c == 'm') {
      if (c == '\n' || c == '\r') { line[lineLen] = '\0'; runLine(line); lineLen = 0; }
      else if (lineLen < sizeof(line) - 1) line[lineLen++] = c;
      continue;
    }
    runKey(c);
  }
}

void setup() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000) {}  // 3 s for USB; never wait forever, or the Romi hangs without a laptop.
  chassis.init();          // Wheels idle at boot; nothing moves until a key arrives.
  Wire.begin();            // Wire BEFORE the PCA9685, or I2C hangs with no error.
  Wire.setWireTimeout(3000, true);  // A stuck I2C bus now resets after 3 ms instead of freezing the Romi.
  pca.begin();
  pca.setPWMFreq(50);      // Hobby servos expect one pulse every 20 ms (50 Hz).
  // The arm jumps to HOME at power-up: keep hands clear of the linkage.
  if (!moveTipNow(HOME_X, HOME_Y)) {
    reportReject(HOME_X, HOME_Y);
    Serial.println(F("Fix HOME_X/HOME_Y or the geometry; the servos stay unpowered."));
    curX = tgtX = HOME_X; curY = tgtY = HOME_Y;
    return;
  }
  tgtX = curX; tgtY = curY;
  Serial.println(F("Ready. m X Y | i/k/j/l jog | w/a/s/d drive | x stop | h home | p print"));
}

void loop() {
  readSerial();
  unsigned long now = millis();
  if (now - lastFrame >= FRAME_MS) { lastFrame = now; stepArm(); }
  if (driving && now - lastDriveKey > DRIVE_HOLD_MS) { chassis.idle(); driving = false; }
  if (now - lastPrint >= PRINT_MS) { lastPrint = now; printPose(); }
}
