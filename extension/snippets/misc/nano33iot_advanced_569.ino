// snippet: nano33iot_advanced_569
// tags: arduinoble, nano33iot
// boards: nano33iot
// library: ArduinoBLE
// description: BLE central scanner: use ArduinoBLE to scan for peripherals advertising service UUID 0xABCD
// prompts: central, scanner, arduinoble, peripherals, advertising, service
// verified: compile-only
// source: kd-generated (id 569)
// exact-include: ArduinoBLE.h

#include <ArduinoBLE.h>

#define SCAN_INTERVAL 100
#define SCAN_DURATION 10000

unsigned long scanStartMs = 0;
unsigned long prevMs = 0;
bool scanning = false;

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }
  BLE.scan();
  scanning = true;
  scanStartMs = millis();
  Serial.println("Scanning...");
}

void loop() {
  if (!scanning) return;

  unsigned long now = millis();

  // Stop scanning after SCAN_DURATION (tracked from scan start, not the throttle timer)
  if (now - scanStartMs >= SCAN_DURATION) {
    BLE.stopScan();
    scanning = false;
    Serial.println("Scan complete");
    return;
  }

  // Throttle discovery prints to SCAN_INTERVAL
  if (now - prevMs >= SCAN_INTERVAL) {
    prevMs = now;

    // During a central scan, discovered peripherals come from BLE.available()
    BLEDevice peripheral = BLE.available();
    if (peripheral) {
      Serial.print("Found device: ");
      Serial.print(peripheral.localName());
      Serial.print(" RSSI: ");
      Serial.println(peripheral.rssi());
    }
  }
}
