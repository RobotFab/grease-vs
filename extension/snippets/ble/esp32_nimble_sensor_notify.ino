// snippet: esp32_nimble_sensor_notify
// tags: ble, bluetooth, esp32, server, peripheral, sensor, notify, dht22, temperature, humidity, nimble, gatt
// boards: esp32
// library: NimBLE-Arduino, DHT sensor library
// description: ESP32 BLE GATT server (NimBLE) that reads a DHT22 temperature/humidity sensor
//   and notifies connected clients every 1 second. Shows: NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY,
//   setValue() with a string, notify(), server callbacks for connect/disconnect/advertise restart.
// prompts: ESP32 BLE sensor notify, NimBLE sensor, ESP32 BLE temperature notify, NimBLE DHT22, ESP32 Bluetooth sensor, BLE notify ESP32 temperature
// exact-include: NimBLEDevice.h, DHT.h
// not-tags: nano33, nano33iot, nano33ble, arduinoble, samd, bleservice, blecharacteristic

#include <NimBLEDevice.h>
#include <DHT.h>

#define DHT_PIN   4
#define DHT_TYPE  DHT22

DHT dht(DHT_PIN, DHT_TYPE);

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHAR_TEMP_UUID      "12345678-1234-5678-1234-56789abcdef1"
#define CHAR_HUMID_UUID     "12345678-1234-5678-1234-56789abcdef2"

NimBLECharacteristic* pTempChar  = nullptr;
NimBLECharacteristic* pHumidChar = nullptr;
bool deviceConnected = false;

// NimBLE-Arduino 2.x signatures. Both callbacks take a NimBLEConnInfo&, and onDisconnect
// also takes the disconnect reason. Writing the 1.x one-argument forms compiles cleanly,
// overrides nothing, and leaves deviceConnected false forever — notify() then never fires.
// Mark them `override` so a future signature change is a compile error, not a silent no-op.
class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& info) override {
    deviceConnected = true;
  }
  void onDisconnect(NimBLEServer* s, NimBLEConnInfo& info, int reason) override {
    deviceConnected = false;
    NimBLEDevice::startAdvertising();  // restart advertising so a new client can connect
  }
};

void setup() {
  Serial.begin(115200);
  dht.begin();

  NimBLEDevice::init("ESP32-Sensor");
  NimBLEServer* server = NimBLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  NimBLEService* service = server->createService(SERVICE_UUID);

  pTempChar = service->createCharacteristic(
    CHAR_TEMP_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  pHumidChar = service->createCharacteristic(
    CHAR_HUMID_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );

  service->start();

  // Advertise the service UUID. Without addServiceUUID() the device is discoverable but a
  // central scanning by service UUID will never match it.
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->enableScanResponse(true);
  adv->start();
  Serial.println("BLE advertising — waiting for client");
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  float temp  = dht.readTemperature();
  float humid = dht.readHumidity();
  if (isnan(temp) || isnan(humid)) {
    Serial.println("DHT read failed");
    return;
  }

  // Format as string so any BLE client can read it without byte-order issues
  char buf[12];
  snprintf(buf, sizeof(buf), "%.1f", temp);
  pTempChar->setValue(buf);
  snprintf(buf, sizeof(buf), "%.1f", humid);
  pHumidChar->setValue(buf);

  if (deviceConnected) {
    pTempChar->notify();
    pHumidChar->notify();
  }

  Serial.printf("Temp: %.1f C  Humid: %.1f %%\n", temp, humid);
}
