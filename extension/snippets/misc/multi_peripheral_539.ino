// snippet: multi_peripheral_539
// tags: servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Conveyor sorter: HC-SR04 (trig=30,echo=31) detects objects
// prompts: conveyor, sorter, hc-sr04, (trig=30, echo=31), detects
// verified: compile-only
// source: kd-generated (id 539)
// exact-include: Servo.h

#include <Servo.h>

const int TRIG_PIN = 30;
const int ECHO_PIN = 31;
const int ENCODER_PIN = 2;
const int SERVO_PIN = 9;

Servo gateServo;
int encoderPos = 0;
int lastEncoderPos = 0;
int objectDetected = 0;
unsigned long prevTriggerTime = 0;
const unsigned long TRIGGER_INTERVAL = 100;  // milliseconds
unsigned long gateOpenTime = 0;
bool gateOpen = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ENCODER_PIN, INPUT);
  gateServo.attach(SERVO_PIN);
  gateServo.write(0);  // Close the gate initially
}

void loop() {
  unsigned long now = millis();

  // Read encoder position with timeout to prevent blocking
  // pulseIn() default timeout is 1 second, which would block the entire loop
  // Adding a shorter timeout (10ms) prevents blocking if encoder signal is missing
  encoderPos = pulseIn(ENCODER_PIN, HIGH, 10000);  // 10ms timeout

  // Check for object using HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout for ~5m range
  int distance = duration * 0.034 / 2;

  if (distance < 10) {  // Adjust threshold as needed
    objectDetected = 1;
  } else {
    objectDetected = 0;
  }

  // Trigger servo gate if object detected and enough time has passed
  if (objectDetected && !gateOpen && now - prevTriggerTime >= TRIGGER_INTERVAL) {
    prevTriggerTime = now;
    gateServo.write(90);  // Open the gate
    gateOpenTime = now;
    gateOpen = true;
  }

  // Non-blocking: close the gate 100 ms after opening (was delay(100))
  if (gateOpen && now - gateOpenTime >= 100) {
    gateServo.write(0);  // Close the gate
    gateOpen = false;
  }
}
