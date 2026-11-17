// snippet: sensor_temp_231
// tags: temperature, sensor, millis, onewire, dallastemperature
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: OneWire, DallasTemperature
// description: One-wire enumeration: scan OneWire bus on pin 3, detect and print addresses of all DS18B20 sensor…
// prompts: one-wire, enumeration, onewire, detect, addresses, ds18b20
// verified: compile-only
// source: kd-generated (id 231)
// exact-include: OneWire.h, DallasTemperature.h

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 3
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

// Declare up to 8 addresses globally
byte addresses[8][8] = {{0}};
int numSensors = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  scanSensors();
  printAddresses();
  readTemperatures();
}

void loop() {
  readTemperatures();
  delay(1000);  // Delay between readings
}

void scanSensors() {
  byte addr[8];
  while (numSensors < 8 && sensors.getAddress(addr, numSensors)) {
    memcpy(addresses[numSensors], addr, 8);
    numSensors++;
  }
}

void printAddresses() {
  Serial.println("Detected DS18B20 sensors:");
  for (int i = 0; i < numSensors; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    for (int j = 0; j < 8; j++) {
      Serial.print(addresses[i][j], HEX);
      if (j < 7) Serial.print(":");
    }
    Serial.println();
  }
}

void readTemperatures() {
  sensors.requestTemperatures();  // Dallas: trigger a conversion on the 1-Wire bus (blocks ~750ms)
  for (int i = 0; i < numSensors; i++) {
    float tempC = sensors.getTempC(addresses[i]);
    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.println(" not found");
    } else {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(tempC);
      Serial.println(" C");
    }
  }
}
