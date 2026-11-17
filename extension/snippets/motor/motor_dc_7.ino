// snippet: motor_dc_7
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read potentiometer on A0, map to motor speed 0–255, drive L298N pins 5/6
// prompts: potentiometer, motor, speed, 0–255, drive, l298n
// verified: compile-only
// source: kd-generated (id 7)

const int POT_PIN = A0;
const int EN = 9;   // PWM enable (ENA on L298N)
const int IN1 = 7;
const int IN2 = 8;

void setup() {
  Serial.begin(9600);
  pinMode(EN, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
}

void loop() {
  static unsigned long printMs = 0;
  const unsigned long PRINT_INTERVAL = 200;

  unsigned long now = millis();
  if (now - printMs >= PRINT_INTERVAL) {
    printMs = now;
    int potValue = analogRead(POT_PIN);
    int motorSpeed = map(potValue, 0, 1023, 0, 255);
    motorForward(motorSpeed);
    Serial.print("Motor Speed: "); Serial.println(motorSpeed);
  }
}

void motorForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, constrain(speed, 0, 255));
}
