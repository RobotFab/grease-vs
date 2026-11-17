// snippet: line_follower_ir
// tags: line-follower, ir, infrared, sensor, l298n, motor, robot, qtr, reflectance, bang-bang, proportional
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: 3-sensor IR array line follower with L298N H-bridge — bang-bang steering and optional P control.
//   Sensors: left=A0, center=A1, right=A2; HIGH = over black line (common for TCRT5000/QTR modules).
//   L298N: ENA=5 (PWM), IN1=6, IN2=7 (left motor); ENB=9 (PWM), IN3=10, IN4=11 (right motor).
// prompts: line follower, IR sensor line follower, follow black line Arduino, line follower robot, 3 sensor line follower, QTR line follower, TCRT5000 line follower, L298N line follower, IR reflectance line follower, bang bang line follower, proportional line follower, robot follow line, black line tracking, white line follower, line tracking robot
// not-tags: encoders, pid, stepper, servo, bluetooth, wifi

// ── pins ──────────────────────────────────────────────────────────────────────
const int IR_L = A0, IR_C = A1, IR_R = A2;
const int ENA = 5, IN1 = 6,  IN2 = 7;   // left motor
const int ENB = 9, IN3 = 10, IN4 = 11;  // right motor

const int BASE_SPEED = 180;   // 0–255
const int TURN_SPEED = 120;   // reduced speed on turning side

// ── helpers ───────────────────────────────────────────────────────────────────
void setLeft(int duty) {
  duty = constrain(duty, 0, 255);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENA, duty);
}

void setRight(int duty) {
  duty = constrain(duty, 0, 255);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENB, duty);
}

void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}

// ── setup ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(IR_L, INPUT); pinMode(IR_C, INPUT); pinMode(IR_R, INPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
}

// ── loop ──────────────────────────────────────────────────────────────────────
void loop() {
  bool l = digitalRead(IR_L);
  bool c = digitalRead(IR_C);
  bool r = digitalRead(IR_R);

  // Bang-bang steering: center on line → straight; veer left/right on deviation
  if (c && !l && !r) {
    // on track — drive straight
    setLeft(BASE_SPEED);
    setRight(BASE_SPEED);
  } else if (r && !l) {
    // drifted right — steer right (slow left wheel)
    setLeft(BASE_SPEED);
    setRight(TURN_SPEED);
  } else if (l && !r) {
    // drifted left — steer left (slow right wheel)
    setLeft(TURN_SPEED);
    setRight(BASE_SPEED);
  } else if (!c && !l && !r) {
    // lost line — stop
    stopMotors();
  } else {
    // all sensors on line (T-junction or start) — drive straight through
    setLeft(BASE_SPEED);
    setRight(BASE_SPEED);
  }

  Serial.print("L:"); Serial.print(l);
  Serial.print(" C:"); Serial.print(c);
  Serial.print(" R:"); Serial.println(r);
}
