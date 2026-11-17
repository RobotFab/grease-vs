// snippet: mlx90614_ir_temp
// tags: temperature, infrared, mlx90614, contactless, ir, i2c, adafruit, non-contact
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit MLX90614
// description: Non-contact IR temperature measurement with MLX90614 (object + ambient)
// prompts: MLX90614 contactless temperature, non-contact IR thermometer, infrared body temperature, remote temp sensor, pyrometer
// exact-include: Wire.h, Adafruit_MLX90614.h

#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!mlx.begin()) { Serial.println("MLX90614 not found"); while (1); }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  Serial.print("Ambient: "); Serial.print(mlx.readAmbientTempC()); Serial.print("°C  ");
  Serial.print("Object: ");  Serial.print(mlx.readObjectTempC());  Serial.println("°C");
}
