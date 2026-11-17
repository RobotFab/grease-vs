// snippet: pico_253
// tags: pico, rp2040, raspberry-pi, servo
// boards: rp2040
// NOTE: pin 0 is hardware Serial RX on AVR — safe on RP2040 only.
// library: Servo
// description: RPi Pico active sketch: PING replies PONG, READ_ADC returns voltage
// prompts: serial handshake Pico, PING PONG RP2040, serial command voltage Pico, active mode Raspberry Pi Pico, read ADC serial Pi Pico, interactive serial RP2040
// verified: compile-only
// source: kd-generated (id 253)
// exact-include: Servo.h

#include <Servo.h>

#define ADC_PIN A0
#define SERVO_PIN 0

Servo myServo;
char cmdBuf[32];
int adcValue = 0;
int servoAngle = 90;
unsigned long prevMs = 0;
const unsigned long STATUS_INTERVAL = 1000;

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);
  pinMode(ADC_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();
  if (Serial.available() > 0) {
    int i = 0;
    while (Serial.available() > 0 && i < 31) {
      cmdBuf[i++] = Serial.read();
    }
    cmdBuf[i] = '\0';

    if (cmdBuf[0] == 'P' && cmdBuf[1] == 'I' && cmdBuf[2] == 'N' && cmdBuf[3] == 'G') {
      Serial.println("PONG");
    } else if (cmdBuf[0] == 'R' && cmdBuf[1] == 'E' && cmdBuf[2] == 'A' && cmdBuf[3] == 'D') {
      adcValue = analogRead(ADC_PIN);
      float voltage = adcValue * (3.3 / 4095.0);
      Serial.print("ADC: "); Serial.print(adcValue);
      Serial.print(" V: "); Serial.println(voltage);
    } else if (cmdBuf[0] == 'S' && cmdBuf[1] == 'E' && cmdBuf[2] == 'T' && cmdBuf[3] == '_') {
      int angle = atoi(&cmdBuf[4]);
      if (angle >= 0 && angle <= 180) {
        servoAngle = angle;
        myServo.write(servoAngle);
      }
    } else if (cmdBuf[0] == 'S' && cmdBuf[1] == 'T' && cmdBuf[2] == 'A' && cmdBuf[3] == 'T') {
      if (now - prevMs >= STATUS_INTERVAL) {
        prevMs = now;
        Serial.print("ADC: "); Serial.print(adcValue);
        Serial.print(" V: "); Serial.print(adcValue * (3.3 / 4095.0));
        Serial.print(" Servo: "); Serial.println(servoAngle);
      }
    }
  }
}
