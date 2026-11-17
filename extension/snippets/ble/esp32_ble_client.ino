// snippet: esp32_ble_client
// tags: ble, bluetooth, esp32, client, central, gatt, remote, characteristic, read, notify, nimble
// boards: esp32
// library: NimBLE-Arduino
// description: ESP32 BLE client (central role) — connects to a server by service UUID,
//   reads a characteristic and subscribes to notifications. (NimBLE stack)
// prompts: ESP32 BLE client, ESP32 BLE central, BLE client ESP32, connect BLE ESP32, read remote characteristic ESP32, BLE notify ESP32 client
// exact-include: NimBLEDevice.h
// not-tags: nano33, nano33iot, nano33ble, arduinoble, samd, bleservice, blecharacteristic

#include <NimBLEDevice.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static NimBLERemoteCharacteristic* pChar = nullptr;
static bool connected = false;

// Set by the scan callback, acted on in loop(). Do NOT connect inside the callback:
// it runs on the NimBLE host task, and blocking there stalls the whole stack.
static bool          doConnect = false;
static NimBLEAddress targetAddr;

void notifyCallback(NimBLERemoteCharacteristic*, uint8_t* data, size_t len, bool) {
  Serial.print("Notify: ");
  for (size_t i = 0; i < len; i++) Serial.print((char)data[i]);
  Serial.println();
}

// NimBLE-Arduino 2.x: onDisconnect carries the disconnect reason. The 1.x one-argument
// form compiles but overrides nothing, so `connected` stays true after the link drops and
// loop() keeps reading a dead characteristic. `override` turns that mistake into an error.
class ClientCallbacks : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient* c) override { connected = true; }
  void onDisconnect(NimBLEClient* c, int reason) override {
    connected = false;
    pChar     = nullptr;   // the remote characteristic dies with the connection
    doConnect = false;
  }
};

bool connectToServer(NimBLEAddress addr) {
  NimBLEClient* client = NimBLEDevice::createClient();
  client->setClientCallbacks(new ClientCallbacks());

  // connect() returns false on failure — ignoring it leaves you calling getService()
  // on a client that never linked up.
  if (!client->connect(addr)) {
    NimBLEDevice::deleteClient(client);
    return false;
  }

  NimBLERemoteService* svc = client->getService(SERVICE_UUID);
  if (!svc) { client->disconnect(); return false; }

  pChar = svc->getCharacteristic(CHARACTERISTIC_UUID);
  if (!pChar) { client->disconnect(); return false; }

  if (pChar->canNotify()) pChar->subscribe(true, notifyCallback);
  return true;
}

class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* dev) {
    if (dev->haveServiceUUID() && dev->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
      NimBLEDevice::getScan()->stop();
      targetAddr = dev->getAddress();
      doConnect  = true;   // hand the work to loop(), do not connect from this callback
    }
  }
};

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("ESP32-Client");
  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setScanCallbacks(new ScanCallbacks());
  scan->setActiveScan(true);
  scan->start(5, false);
}

void loop() {
  if (doConnect) {
    doConnect = false;
    if (!connectToServer(targetAddr)) {
      Serial.println("Connect failed — rescanning");
      NimBLEDevice::getScan()->start(5, false);
    }
  }

  // Poll the characteristic at 1 Hz without delay() — a blocking loop cannot also
  // service reconnects or anything else the robot needs to do.
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  if (connected && pChar && pChar->canRead()) {
    std::string val = pChar->readValue();
    Serial.print("Read: "); Serial.println(val.c_str());
  } else if (!connected) {
    // Link dropped: onDisconnect cleared the flags, so go looking again.
    NimBLEDevice::getScan()->start(5, false);
  }
}
