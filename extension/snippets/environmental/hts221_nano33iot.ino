// snippet: hts221_nano33iot
// tags: hts221, temperature, humidity, nano33iot, arduino-hts221, samd, builtin
// not-tags: DHT11, DHT22, BME280, SHT31, sparkfun, uno, mega, avr
// boards: nano33iot
// library: Arduino_HTS221
// description: Read temperature and humidity from the built-in HTS221 sensor on Nano 33 IoT.
//   Arduino_HTS221 API: HTS.begin() → bool, HTS.readTemperature() → float (°C),
//   HTS.readHumidity() → float (%). The sensor is soldered onto the Nano 33 IoT board —
//   no wiring needed. #include <Arduino_HTS221.h> only; Wire.h is NOT required.
//   Do NOT use DHT.h, Adafruit_HTU21D, or SparkFun libraries — they are wrong for this board.
// prompts: HTS221 Nano 33 IoT temperature humidity, Arduino_HTS221, built-in sensor nano33, read temp humidity nano33iot, HTS.readTemperature, HTS.readHumidity
// exact-include: Arduino_HTS221.h

#include <Arduino_HTS221.h>

unsigned long prevMs = 0;
const unsigned long INTERVAL = 1000;  // 1 Hz

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!HTS.begin()) {
    Serial.println("HTS221 init failed");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  float temp = HTS.readTemperature();
  float hum  = HTS.readHumidity();

  Serial.print("Temperature: ");
  Serial.println(temp, 1);
  Serial.print("Humidity: ");
  Serial.println(hum, 1);
}
