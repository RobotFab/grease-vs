// snippet: esp32_active_252
// tags: esp32, active, serial, command, arduinoble
// boards: nano33iot
// library: ArduinoBLE
// description: Active sketch: BLE_NOTIFY ON starts sending sensor readings as BLE notifications every 500ms
// prompts: esp32, active, sketch, ble_notify, starts, sending
// verified: compile-only
// source: kd-generated (id 252)
// exact-include: ArduinoBLE.h

// ArduinoBLE works on nano33iot; ESP32 typically uses BLEDevice.h (NimBLE)
#include <ArduinoBLE.h>

BLEService sensorService("1101");
BLEFloatCharacteristic valueChar("2101", BLERead | BLENotify);

bool notifyEnabled = false;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }
  BLE.setLocalName("SensorNode");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(valueChar);
  BLE.addService(sensorService);
  BLE.advertise();
  Serial.println("Send: BLE_NOTIFY ON / OFF");
}

void loop() {
  BLE.poll();

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "BLE_NOTIFY ON") {
      notifyEnabled = true;
    } else if (cmd == "BLE_NOTIFY OFF") {
      notifyEnabled = false;
    }
  }

  if (notifyEnabled && millis() - prevMs >= 500) {
    prevMs = millis();
    // nano33iot: 12-bit ADC, 3.3V reference
    float val = analogRead(A0) * (3.3f / 4095.0f);
    valueChar.writeValue(val);
    Serial.print("BLE notify: ");
    Serial.println(val);
  }
}
