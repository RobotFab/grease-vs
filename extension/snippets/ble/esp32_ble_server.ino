// snippet: esp32_ble_server
// tags: ble, bluetooth, esp32, server, characteristic, gatt, notify, arduino, nimble, callback, onconnect, override, nimbleconninfo
// boards: esp32
// library: NimBLE-Arduino
// description: BLE GATT server on ESP32 with a notify characteristic (NimBLE-Arduino 2.x).
//   Server callbacks in 2.x take a NimBLEConnInfo&, and onDisconnect also takes an int reason:
//     void onConnect(NimBLEServer*, NimBLEConnInfo&)                <- 2 arguments
//     void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int reason) <- 3 arguments
//   The 1.x one-argument forms compile without error or warning and then never fire, because
//   they override nothing. Mark every callback `override` so the compiler catches it.
//   Notify only while a client is subscribed, and pace it — notifying every loop floods the
//   connection interval and packets get dropped.
// prompts: ESP32 BLE server, Bluetooth ESP32, BLE GATT ESP32, BLE characteristic ESP32, ESP32 Bluetooth notify, NimBLE onConnect signature, NimBLE server callback never called
// exact-include: NimBLEDevice.h
// not-tags: nano33, nano33iot, nano33ble, arduinoble, samd, bleservice, blecharacteristic

#include <NimBLEDevice.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

NimBLECharacteristic *pChar;
bool deviceConnected = false;
unsigned long prevMs = 0;

// NimBLE 2.x signatures. The 1.x one-argument forms still compile — C++ makes them new,
// unrelated methods — so the callback silently never fires. `override` makes that an error.
class ConnCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& connInfo) override {
    deviceConnected = true;
  }
  void onDisconnect(NimBLEServer* s, NimBLEConnInfo& connInfo, int reason) override {
    deviceConnected = false;
    NimBLEDevice::startAdvertising();   // or the device is invisible to the next client
  }
};

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("ESP32-Sensor");
  NimBLEServer *server = NimBLEDevice::createServer();
  server->setCallbacks(new ConnCallbacks());

  NimBLEService *service = server->createService(SERVICE_UUID);
  pChar = service->createCharacteristic(
    CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  service->start();
  server->getAdvertising()->start();
}

void loop() {
  unsigned long now = millis();
  if (deviceConnected && now - prevMs >= 500) {
    prevMs = now;
    float val = random(100) / 10.0;
    pChar->setValue(val);
    pChar->notify();
  }
}
