// snippet: motor_stepper_352
// tags: motor, stepper, direction, eeprom, accelstepper
// not-tags: 28byj48, uln2003, 4wire, 2048
// boards: avr, esp32, megaavr, romi32u4, rp2040
// library: EEPROM, AccelStepper
// description: Save the last stepper position to EEPROM (address 0, int32_t) when a button on pin 8 is pressed
// prompts: stepper, position, eeprom, (address, int32_t), button
// verified: compile-only
// source: kd-generated (id 352)
// exact-include: EEPROM.h, AccelStepper.h

#include <EEPROM.h>
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 4);
const int BUTTON_PIN = 8;
int32_t savedPosition = 0;

// ESP32 and RP2040 use flash-emulated EEPROM: you MUST call begin(size)
// before get/put and commit() after put, or data is never persisted.
// AVR's native EEPROM needs neither — begin()/commit() are no-ops there.
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #define EEPROM_SIZE 64  // enough for a few int32_t values
#endif

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  EEPROM.begin(EEPROM_SIZE);  // required on flash-emulated EEPROM cores
#endif

  EEPROM.get(0, savedPosition);  // EEPROM: read any type/struct
  stepper.moveTo(savedPosition);  // absolute target position (steps); call ONCE, not every loop
  stepper.enableOutputs();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    int32_t pos = stepper.currentPosition();  // current position in steps (no currentSpeed() method exists)
    EEPROM.put(0, pos);  // EEPROM: write any type/struct (handles size)

#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
    EEPROM.commit();  // flush to flash — without this, put() is lost on power-off
#endif

    // Debounce: wait for button release (bounded to avoid blocking forever)
    unsigned long tDb = millis();
    while (digitalRead(BUTTON_PIN) == LOW) {
      if (millis() - tDb > 1000) break;
    }
  }
  stepper.run();  // MUST be called every loop iteration to step the motor
}
