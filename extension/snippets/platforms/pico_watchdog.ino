// snippet: pico_watchdog
// tags: pico, rp2040, raspberry-pi, watchdog, timer, hardware-timer, repeating-timer, wdt, reset, timeout, add_repeating_timer_ms
// boards: rp2040
// library: none
// description: RPi Pico: hardware watchdog with 2-second timeout using Watchdog.h (Arduino-Pico core).
//   Also shows add_repeating_timer_ms() for precise periodic callbacks — RP2040-specific SDK call.
//   Watchdog.h is part of the Arduino-Pico board core (earle philhower) — NOT available on AVR/ESP32.
//   NOT: wdt.h / wdt_enable() — that is the AVR watchdog API, not RP2040.
//   NOT: esp_task_wdt_init() — that is the ESP32 watchdog API.
//   add_repeating_timer_ms(ms, callback, data, &timer): fires callback on hardware alarm — runs in IRQ context.
//   Watchdog.enable(ms): arms watchdog. Watchdog.reset(): feeds it. If not fed within ms, MCU resets.
// prompts: watchdog Pico, WDT RP2040, hardware watchdog Raspberry Pi Pico, Watchdog.enable Pico, 2 second timeout Pico, add_repeating_timer_ms Pico, hardware timer RP2040, repeating timer Pico, periodic callback RP2040, Watchdog.h Pico
// not-tags: esp32, avr, uno, mega, nano33iot
// exact-include: Arduino.h, hardware/watchdog.h

#include <Arduino.h>
#include <hardware/watchdog.h>  // RP2040 SDK header — part of Arduino-Pico core; not needed on other boards

// add_repeating_timer_ms callback: runs in IRQ context — keep short, no Serial, no malloc
static repeating_timer_t feedTimer;

bool feedWatchdog(repeating_timer_t* rt) {
  // Feed watchdog from timer ISR — prevents reset as long as this fires every <2s
  watchdog_update();  // RP2040 SDK call (lower level than Watchdog.reset())
  return true;        // return true to keep the timer repeating
}

unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);

  // arm watchdog with 2000 ms timeout — MCU resets if not fed within that window
  watchdog_enable(2000, true);  // true = pause watchdog during debug halt

  // Schedule a repeating timer to feed the watchdog every 1500 ms
  // add_repeating_timer_ms(interval_ms, callback, user_data, timer_struct)
  add_repeating_timer_ms(1500, feedWatchdog, nullptr, &feedTimer);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;
    Serial.println("running");
  }
  // No manual Watchdog.reset() needed here — the hardware timer handles it
}
