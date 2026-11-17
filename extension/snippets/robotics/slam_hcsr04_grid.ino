// snippet: slam_hcsr04_grid
// tags: slam, mapping, occupancy-grid, hcsr04, ultrasonic, servo, robot, navigation
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: NewPing, Servo
// description: Occupancy grid SLAM with rotating HC-SR04 — scan, mark cells, print ASCII map
// prompts: SLAM ultrasonic mapping, occupancy grid HC-SR04, rotating ultrasonic scan, ASCII occupancy map, HC-SR04 SLAM
// exact-include: NewPing.h, Servo.h

#include <NewPing.h>
#include <Servo.h>

// ---- CONFIGURE ----
const int TRIG      = 7, ECHO = 6;
const int SERVO_PIN = 9;
const int MAX_CM    = 200;
const float CELL_CM = 10.0f;  // grid cell size
const int  GRID_W   = 20, GRID_H = 20;
// -------------------

NewPing sonar(TRIG, ECHO, MAX_CM);
Servo   scanServo;

// 0=unknown, 1=free, 2=occupied
uint8_t grid[GRID_H][GRID_W];
const int ORIGIN_X = GRID_W / 2, ORIGIN_Y = GRID_H / 2;

void markRay(int angleDeg, float distCm) {
  float rad = (angleDeg - 90) * PI / 180.0f;  // 0°=right, 90°=forward
  int steps = (int)(distCm / CELL_CM);

  for (int s = 0; s < steps && s < 15; s++) {
    int gx = ORIGIN_X + (int)(s * cos(rad));
    int gy = ORIGIN_Y - (int)(s * sin(rad));  // y-axis flip for display
    if (gx >= 0 && gx < GRID_W && gy >= 0 && gy < GRID_H) grid[gy][gx] = 1;
  }
  // mark obstacle
  int ox = ORIGIN_X + (int)(steps * cos(rad));
  int oy = ORIGIN_Y - (int)(steps * sin(rad));
  if (ox >= 0 && ox < GRID_W && oy >= 0 && oy < GRID_H) grid[oy][ox] = 2;
}

void printMap() {
  Serial.println("--- MAP ---");
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      char c = (x == ORIGIN_X && y == ORIGIN_Y) ? 'R' :
               (grid[y][x] == 2) ? '#' :
               (grid[y][x] == 1) ? '.' : ' ';
      Serial.print(c);
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200);
  scanServo.attach(SERVO_PIN);
  memset(grid, 0, sizeof(grid));
}

void loop() {
  // full 180° sweep
  for (int ang = 0; ang <= 180; ang += 5) {
    scanServo.write(ang);
    delay(100);
    int dist = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    if (dist > 0) markRay(ang, dist);
  }
  printMap();
  delay(500);
}
