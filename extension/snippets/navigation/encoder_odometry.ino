// snippet: encoder_odometry
// tags: encoder, odometry, navigation, differential, drive, position, heading, wheel, robot, dead-reckoning
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// description: Differential-drive odometry from two quadrature wheel encoders.
//   dTheta = (dRight - dLeft) / wheelbase
//   dx = dCenter * cos(theta + dTheta/2),  dy = dCenter * sin(theta + dTheta/2)
//     — integrate about the MIDPOINT heading of the interval, not the end heading.
//   All position state (x, y, theta) must be declared globally and volatile-snapshotted.
//   READ THE B CHANNEL. A single-channel RISING interrupt can only count up, so a robot
//   driving in reverse integrates its pose FORWARD. Each ISR must read the other channel
//   to get the sign: ticks += digitalRead(B) ? +1 : -1.
//   NOT for the Romi 32U4: pins 2 and 3 are the Romi's I2C bus (SDA/SCL), and its encoders
//   are already counted in hardware by Romi32U4Encoders. Use romi32u4_dead_reckoning there.
//   To DRIVE a set distance rather than just track one, terminate on a count target —
//   see romi32u4_fsm_route_drive. Never substitute a millis() timeout for distance.
// prompts: encoder odometry, differential drive odometry, wheel encoder position, dead reckoning, robot position tracking, encoder navigation, track robot position, differential drive heading, encoder distance
// not-tags: romi32u4, romi

// Channel A must be interrupt-capable (pins 2, 3 on AVR). Channel B can be any digital pin —
// it is only read inside the ISR to recover the direction of travel.
#define ENC_L_A 2
#define ENC_L_B 4
#define ENC_R_A 3
#define ENC_R_B 5

// Robot physical parameters
const float WHEEL_DIAM   = 0.065;    // wheel diameter in meters
const float WHEELBASE    = 0.15;     // distance between wheels in meters
const int   TICKS_PER_REV = 360;     // encoder ticks per wheel revolution
const float METERS_PER_TICK = (PI * WHEEL_DIAM) / TICKS_PER_REV;

volatile long ticksL = 0, ticksR = 0;
long prevTicksL = 0, prevTicksR = 0;
float x = 0.0, y = 0.0, theta = 0.0;   // pose in meters and radians
unsigned long prevMs = 0;

// On a rising edge of A, the level of B tells you which way the wheel turned.
// Without this the count is unsigned and reverse motion is integrated as forward motion.
void isrLeft()  { ticksL += digitalRead(ENC_L_B) ? 1 : -1; }
void isrRight() { ticksR += digitalRead(ENC_R_B) ? -1 : 1; }  // mirrored: right wheel faces the other way

void setup() {
  Serial.begin(115200);
  pinMode(ENC_L_A, INPUT_PULLUP);
  pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP);
  pinMode(ENC_R_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_L_A), isrLeft,  RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_R_A), isrRight, RISING);
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;   // 20 Hz odometry update
  prevMs = now;

  // Snapshot ticks atomically
  noInterrupts();
  long curL = ticksL;
  long curR = ticksR;
  interrupts();

  long dTicksL = curL - prevTicksL;
  long dTicksR = curR - prevTicksR;
  prevTicksL = curL;
  prevTicksR = curR;

  // Convert ticks to meters
  float dLeft  = dTicksL * METERS_PER_TICK;
  float dRight = dTicksR * METERS_PER_TICK;
  float dCenter = (dLeft + dRight) / 2.0;
  float dTheta  = (dRight - dLeft) / WHEELBASE;

  // Update pose about the MIDPOINT heading of the interval — using the end heading
  // biases every turn and the error accumulates over a run.
  float midTheta = theta + dTheta / 2.0;
  x += dCenter * cos(midTheta);
  y += dCenter * sin(midTheta);
  theta += dTheta;

  // Keep heading in (-PI, PI] so it does not drift toward float precision loss.
  if (theta >  PI) theta -= 2.0 * PI;
  if (theta <= -PI) theta += 2.0 * PI;

  Serial.print("x: ");     Serial.print(x, 4);
  Serial.print(" y: ");     Serial.print(y, 4);
  Serial.print(" theta: "); Serial.println(theta * 180.0 / PI, 2);
}
