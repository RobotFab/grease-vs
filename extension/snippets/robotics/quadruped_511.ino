// snippet: quadruped_511
// tags: quadruped, robot, battery, monitor, lipo, voltage, divider, low-battery, alert, led, 3s, cutoff
// boards: esp32
// library: ESP32Servo
// description: Quadruped battery monitor: read 3S LiPo voltage via voltage divider on A0, halt on low battery.
//   ESP32 ADC: 12-bit (0–4095), reference 3.3 V — NOT 5V/1023 as on AVR.
//   analogRead(A0) on ESP32 → 0–4095. Voltage = reading * 3.3 / 4095 * divider_ratio.
//   NOT: analogRead(A0) * 5.0 / 1023 — that is the AVR (Uno/Mega) formula, wrong on ESP32.
//   3S LiPo cutoff: 3.0 V/cell = 9.0 V total. Safe cutoff with margin: ~10.5 V.
//   Voltage divider ÷4: R1=30kΩ, R2=10kΩ → Vout = Vin * 10/(30+10) = Vin/4. Max Vin = 3.3*4 = 13.2 V.
// prompts: quadruped battery monitor, lipo voltage ESP32, 3S lipo cutoff, battery low alert quadruped, voltage divider ESP32 ADC, robot battery monitor, lipo battery check, low battery halt servo, ESP32 analog voltage divider, 3S lipo 10.5V cutoff
// exact-include: Arduino.h, ESP32Servo.h

#include <Arduino.h>
#include <ESP32Servo.h>

const int BATTERY_PIN = A0;
const int LED_PIN     = 2;     // ESP32 onboard LED (GPIO2 on most DevKit boards)
const int SERVO_PIN   = 13;    // Servo signal pin
const int MIN_MV      = 10500; // 10.5 V cutoff in millivolts (3S LiPo safe minimum)

Servo myServo;
int servoPos = 90;             // Neutral position
bool isBattLow = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(servoPos);
}

void loop() {
  // ESP32 ADC: 12-bit (0–4095), 3.3 V reference — NOT 5.0/1023 (AVR formula)
  int raw = analogRead(BATTERY_PIN);
  // Voltage divider ÷4 (R1=30kΩ, R2=10kΩ): Vbatt = Vadc * 4
  // millivolts: raw * 3300 / 4095 * 4 ≈ raw * 3.22
  int battMv = (int)((long)raw * 3300 * 4 / 4095);

  if (battMv < MIN_MV) {
    if (!isBattLow) {
      isBattLow = true;
      stopMotion();
    }
  } else {
    isBattLow = false;
  }

  blinkLED();
  delay(100);
}

void stopMotion() {
  // Disable PCA9685 / set all servos to safe neutral — extend for your hardware
  Serial.println("LOW BATTERY — motion halted");
}

void blinkLED() {
  static unsigned long blinkTime = 0;
  static bool ledState = false;

  if (millis() - blinkTime >= 500) {
    blinkTime = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}
