// snippet: sensor_loadcell_404
// tags: hx711
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: HX711
// description: Read two HX711 load cells: left (DOUT=3, CLK=4) and right (DOUT=5, CLK=6)
// prompts: hx711, cells, (dout=3, clk=4), right, (dout=5
// verified: compile-only
// source: kd-generated (id 404)
// exact-include: HX711.h

#include <HX711.h>

#define DOUT_LEFT 3
#define CLK_LEFT  4
#define DOUT_RIGHT 5
#define CLK_RIGHT  6

HX711 scaleLeft;
HX711 scaleRight;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  scaleLeft.begin(DOUT_LEFT, CLK_LEFT);
  scaleRight.begin(DOUT_RIGHT, CLK_RIGHT);
  scaleLeft.set_scale(2280.0);  // calibrate with known weight
  scaleRight.set_scale(2280.0);  // calibrate with known weight
  scaleLeft.tare();  // HX711: zero the scale (averaged reading -> offset)
  scaleRight.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scales ready — place weights");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 250) return;
  prevMs = now;

  if (scaleLeft.is_ready() && scaleRight.is_ready()) {
    float leftWeight = scaleLeft.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    float rightWeight = scaleRight.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
    float totalWeight = leftWeight + rightWeight;

    Serial.print("Left: ");
    Serial.print(leftWeight);
    Serial.print(" g, Right: ");
    Serial.print(rightWeight);
    Serial.print(" g, Total: ");
    Serial.print(totalWeight);
    Serial.print(" g, ");

    if (abs(leftWeight - rightWeight) > 10) {
      Serial.println("IMBALANCE");
    } else {
      Serial.println("Balanced");
    }
  }
}
