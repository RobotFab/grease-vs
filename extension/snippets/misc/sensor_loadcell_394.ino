// snippet: sensor_loadcell_394
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Detect when weight placed on the HX711 (DOUT=3, CLK=4) exceeds 500 g
// prompts: detect, weight, placed, hx711, (dout=3, clk=4)
// verified: compile-only
// source: kd-generated (id 394)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_PIN 3
#define CLK_PIN  4
#define LED_PIN  13

HX711 scale;
float currentWeight = 0.0;
bool isOverweight = false;
unsigned long blinkPrevMs = 0;
unsigned long printPrevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  if (scale.is_ready()) {
    currentWeight = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    Serial.print("weight: ");
    Serial.println(currentWeight);

    if (currentWeight > 500.0) {
      isOverweight = true;
    } else {
      isOverweight = false;
    }
  }

  unsigned long now = millis();

  if (isOverweight) {
    if (now - blinkPrevMs >= 250) {
      blinkPrevMs = now;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  if (now - printPrevMs >= 200) {
    printPrevMs = now;
    Serial.print("current weight: ");
    Serial.println(currentWeight);
  }
}
