// snippet: slam_odometry_grid
// tags: slam, odometry, encoder, occupancy-grid, robot, navigation, position, mapping
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Dead-reckoning odometry + ultrasonic occupancy grid — tracks robot XY pose on a map
// prompts: odometry SLAM dead reckoning, encoder occupancy grid, wheel encoder mapping, robot position tracking map

// ---- CONFIGURE ----
const float WHEEL_DIAM_MM  = 65.0f;
const float TRACK_MM       = 150.0f;
const int   ENCODER_CPR    = 360;     // counts per revolution
const int   TRIG_PIN = 4, ECHO_PIN = 5;
const int   CELL_MM  = 50;
const int   GRID_SZ  = 30;
// -------------------

// encoder counts (incremented by interrupt)
volatile long encL = 0, encR = 0;
long prevEncL = 0, prevEncR = 0;

float poseX = (GRID_SZ / 2) * CELL_MM;  // mm
float poseY = (GRID_SZ / 2) * CELL_MM;
float poseH = 0.0f;                      // radians, 0 = +X

uint8_t grid[GRID_SZ][GRID_SZ];

long pulseToCm(long pulses) {
  return pulses * WHEEL_DIAM_MM * PI / ENCODER_CPR;
}

void updateOdometry() {
  long dL = encL - prevEncL;
  long dR = encR - prevEncR;
  prevEncL = encL; prevEncR = encR;

  float distL = (float)dL * WHEEL_DIAM_MM * PI / ENCODER_CPR;
  float distR = (float)dR * WHEEL_DIAM_MM * PI / ENCODER_CPR;
  float dist  = (distL + distR) * 0.5f;
  float dh    = (distR - distL) / TRACK_MM;

  poseH += dh;
  poseX += dist * cos(poseH);
  poseY += dist * sin(poseH);
}

float readUltrasonicCm() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long t = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
  return t * 0.034f / 2.0f;
}

void markObstacle(float dist) {
  float ox = poseX + dist * cos(poseH) * 10.0f;  // mm
  float oy = poseY + dist * sin(poseH) * 10.0f;
  int gx = (int)(ox / CELL_MM); int gy = (int)(oy / CELL_MM);
  if (gx >= 0 && gx < GRID_SZ && gy >= 0 && gy < GRID_SZ) grid[gy][gx] = 2;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  memset(grid, 1, sizeof(grid));  // all free
}

unsigned long prevMs = 0;

void loop() {
  updateOdometry();

  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  float d = readUltrasonicCm();
  if (d > 0 && d < 200) markObstacle(d);

  // print robot pose
  Serial.print("X="); Serial.print(poseX, 0);
  Serial.print(" Y="); Serial.print(poseY, 0);
  Serial.print(" H="); Serial.print(poseH * 180.0f / PI, 1);
  Serial.print("  dist="); Serial.println(d, 1);
}
