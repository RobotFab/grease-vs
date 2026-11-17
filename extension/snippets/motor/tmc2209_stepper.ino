// snippet: tmc2209_stepper
// tags: stepper, tmc2209, trinamic, uart, dir, microstepping, silent, driver
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: leonardo, romi32u4, mega, esp32, rp2040
// library: TMCStepper
// description: Silent stepper control with TMC2209 via UART — configure current and microsteps
// prompts: TMC2209 stepper driver, silent stepper motor, Trinamic UART control, stealthChop quiet stepper, TMCStepper library
// exact-include: TMCStepper.h

#include <TMCStepper.h>

#define STEP_PIN   4
#define DIR_PIN    5
#define EN_PIN     8
#define R_SENSE  0.11f

TMC2209Stepper driver(&Serial1, R_SENSE, 0b00);  // address 0b00 (MS1/MS2 = GND)

unsigned long prevMs = 0;
bool stepState = false;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN,  OUTPUT);
  pinMode(EN_PIN,   OUTPUT);
  digitalWrite(EN_PIN, LOW);  // enable driver

  driver.begin();
  driver.rms_current(600);    // mA
  driver.microsteps(16);
  driver.en_spreadCycle(false);  // stealthChop
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1) return;  // ~1kHz step rate
  prevMs = now;

  stepState = !stepState;
  digitalWrite(STEP_PIN, stepState);
}
