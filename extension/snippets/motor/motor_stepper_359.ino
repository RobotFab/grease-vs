// snippet: motor_stepper_359
// tags: motor, stepper, direction, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: AccelStepper
// description: Monitor an NTC thermistor on A0
// prompts: monitor, thermistor, temperature, exceeds, reduce, accelstepper
// verified: compile-only
// source: kd-generated (id 359)
// exact-include: AccelStepper.h

#include <AccelStepper.h>

const int NTC_PIN = A0;
const int STEPPER_DIR_PIN = 4;
const int STEPPER_STEP_PIN = 3;

const float SERIES_R = 10000.0;
const float NOMINAL_R = 10000.0;
const float NOMINAL_T = 25.0;
const float B_COEFF = 3950.0;

AccelStepper stepper(AccelStepper::DRIVER, STEPPER_STEP_PIN, STEPPER_DIR_PIN);

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(2000);  // Full speed
  stepper.setAcceleration(1000);
  stepper.moveTo(0);  // Start at position 0
}

void loop() {
  static unsigned long tempCheckMs = 0;
  const unsigned long TEMP_CHECK_INTERVAL = 1000;  // Check temperature every second

  if (millis() - tempCheckMs >= TEMP_CHECK_INTERVAL) {
    tempCheckMs = millis();
    float tempC = readNTCCelsius(NTC_PIN);

    if (tempC > 60.0) {
      stepper.setMaxSpeed(100);  // Reduce speed
      Serial.println("Warning: Temperature exceeds 60 °C");
    } else if (tempC < 50.0) {
      stepper.setMaxSpeed(2000);  // Restore full speed
      Serial.println("Temperature is below 50 °C");
    }
  }

  stepper.run();  // MUST be called every loop iteration to step the motor
}

float readNTCCelsius(int pin) {
  int raw = analogRead(pin);
  float r = SERIES_R * (1023.0 / raw - 1.0);
  float t = 1.0 / (1.0 / (NOMINAL_T + 273.15) + log(r / NOMINAL_R) / B_COEFF);
  return t - 273.15;
}
