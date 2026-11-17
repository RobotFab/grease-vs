// snippet: differential_drive
// tags: differential-drive, robot, motor, encoder, pid, kinematics, heading, straight
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Differential drive robot — straight-line hold using encoder feedback (simple P control)
// prompts: differential drive, two wheel robot, tank drive, robot straight line, encoder motor control, wheeled robot, left right motor robot

// ---- CONFIGURE ----
const int ENA = 5, IN1 = 4, IN2 = 9;   // left motor (pins valid on AVR & ESP32, off flash-reserved 6-11)
const int ENB = 10, IN3 = 12, IN4 = 13; // right motor
const int ENC_L = 2, ENC_R = 3;        // encoder A channels (interrupt-capable on AVR & ESP32)
const float Kp  = 2.0f;               // proportional gain
const int BASE_SPEED = 180;
// -------------------

volatile long encLeft = 0, encRight = 0;

// AVR ISRs are plain functions — no IRAM_ATTR (that attribute is ESP32-only and
// undefined on AVR, which breaks the build with "expected initializer").
void iL() { encLeft++;  }
void iR() { encRight++; }

void setMotors(int l, int r) {
  l = constrain(l, -255, 255);
  r = constrain(r, -255, 255);
  digitalWrite(IN1, l >= 0); digitalWrite(IN2, l < 0);
  analogWrite(ENA, abs(l));
  digitalWrite(IN3, r >= 0); digitalWrite(IN4, r < 0);
  analogWrite(ENB, abs(r));
}

void setup() {
  Serial.begin(9600);
  // Configure motor driver pins as outputs
  const int pins[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int i = 0; i < 6; i++) pinMode(pins[i], OUTPUT);
  pinMode(ENC_L, INPUT_PULLUP); pinMode(ENC_R, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_L), iL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_R), iR, RISING);
}

unsigned long prevMs = 0;

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;
  prevMs = now;

  // Atomically read encoder counts
  noInterrupts();
  long l = encLeft; long r = encRight;
  interrupts();

  // P control: if left > right, slow left / speed right to correct drift
  float err = (float)(l - r);
  int leftSpeed  = BASE_SPEED - (int)(Kp * err);
  int rightSpeed = BASE_SPEED + (int)(Kp * err);
  setMotors(leftSpeed, rightSpeed);

  Serial.print("L="); Serial.print(l);
  Serial.print(" R="); Serial.print(r);
  Serial.print(" err="); Serial.println(err, 0);
}
